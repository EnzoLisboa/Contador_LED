#include <stdio.h>
#include <stdlib.h>
#include "pico/bootrom.h"
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "matriz_leds.h"

// Arquivo .pio para controle da matriz
#include "pio_matrix.pio.h"

// Pino que realizará a comunicação do microcontrolador com a matriz
#define OUT_PIN 7

// Pino do led
#define LED_VERMELHO_PIN 13

// Pinos dos botões
#define BUTTON_A_PIN 5
#define BUTTON_B_PIN 6
#define BUTTON_C_PIN 22

// Definir constantes para os valores RGB
#define RGB_OFF {0.0, 0.0, 0.0}
#define RGB_BLUE {0.0, 0.0, 1.0}

// Variáveis globais
int numero = 0; // Inicia o contador em 0
bool modo = false; // Inicia sem modo bootsel
static volatile uint32_t last_time = 0; // Define o tempo inicial (em microssegundos)

// Funções para matriz de LEDs
uint32_t gerar_binario_cor(double red, double green, double blue)
{
  unsigned char RED, GREEN, BLUE;
  RED = red * 255.0;
  GREEN = green * 255.0;
  BLUE = blue * 255.0;
  return (GREEN << 24) | (RED << 16) | (BLUE << 8);
}

uint configurar_matriz(PIO pio){
    bool ok;

    // Define o clock para 128 MHz, facilitando a divisão pelo clock
    ok = set_sys_clock_khz(128000, false);

    printf("iniciando a transmissão PIO");
    if (ok) printf("clock set to %ld\n", clock_get_hz(clk_sys));

    //configurações da PIO
    uint offset = pio_add_program(pio, &pio_matrix_program);
    uint sm = pio_claim_unused_sm(pio, true);
    pio_matrix_program_init(pio, sm, offset, OUT_PIN);

    return sm;
}

void imprimir_desenho(Matriz_leds_config configuracao, PIO pio, uint sm){
    for (int contadorLinha = 4; contadorLinha >= 0; contadorLinha--){
        if(contadorLinha % 2){
            for (int contadorColuna = 0; contadorColuna < 5; contadorColuna ++){
                uint32_t valor_cor_binario = gerar_binario_cor(
                    configuracao[contadorLinha][contadorColuna].red,
                    configuracao[contadorLinha][contadorColuna].green,
                    configuracao[contadorLinha][contadorColuna].blue
                );

                pio_sm_put_blocking(pio, sm, valor_cor_binario);
            }
        }else{
            for (int contadorColuna = 4; contadorColuna >= 0; contadorColuna --){
                uint32_t valor_cor_binario = gerar_binario_cor(
                    configuracao[contadorLinha][contadorColuna].red,
                    configuracao[contadorLinha][contadorColuna].green,
                    configuracao[contadorLinha][contadorColuna].blue
                );

                pio_sm_put_blocking(pio, sm, valor_cor_binario);
            }
        }
    }
}

// Função de interrupção
void gpio_irq_handler(uint gpio, uint32_t events)
{
    // Obtém o tempo atual em microssegundos
    uint32_t current_time = to_us_since_boot(get_absolute_time());
    // Verifica se passou tempo suficiente desde o último evento
    if (current_time - last_time > 200000) // 200 ms de debouncing
    {
        last_time = current_time; // Atualiza o tempo do último evento

        // Verifica qual botão foi pressionado
        if (gpio == BUTTON_A_PIN)
        {
            numero++;
            if (numero > 9) {numero--;} //Limita o número
            printf("Número: %d\n", numero);
        }
        else if (gpio == BUTTON_B_PIN)
        {
            numero--;
            if (numero < 0) {numero++;} //Limita o número
            printf("Número: %d\n", numero);
        }
        else if (gpio == BUTTON_C_PIN){modo=!modo;}
    }
}

// Função para piscar o led
void piscar_led()
{
    // Inicializa o led vermelho
    gpio_init(LED_VERMELHO_PIN);
    gpio_set_dir(LED_VERMELHO_PIN, GPIO_OUT);
    // Liga o LED
    gpio_put(LED_VERMELHO_PIN, 1);
    sleep_ms(100); // Espera 100 milissegundos
    // Desliga o LED
    gpio_put(LED_VERMELHO_PIN, 0);
    sleep_ms(100); // Espera 100 milissegundos
}

// Função para configurar os botões
void configurar_botao(uint botao)
{
    gpio_init(botao);
    gpio_set_dir(botao, GPIO_IN);
    gpio_pull_up(botao);
}

// Função para desligar os leds e entrar em bootsel
void desligar(PIO pio, uint sm)
{
    Matriz_leds_config matriz_desl = {
        {RGB_OFF, RGB_OFF, RGB_OFF, RGB_OFF, RGB_OFF}, // Linha 0
        {RGB_OFF, RGB_OFF, RGB_OFF, RGB_OFF, RGB_OFF}, // Linha 1
        {RGB_OFF, RGB_OFF, RGB_OFF, RGB_OFF, RGB_OFF}, // Linha 2
        {RGB_OFF, RGB_OFF, RGB_OFF, RGB_OFF, RGB_OFF}, // Linha 3
        {RGB_OFF, RGB_OFF, RGB_OFF, RGB_OFF, RGB_OFF}, // Linha 4
    };
    imprimir_desenho(matriz_desl, pio, sm);
    gpio_put(LED_VERMELHO_PIN, 0);
    sleep_ms(200);
    reset_usb_boot(0, 0);
}

// Função genérica para imprimir números
void imprimir_numero(PIO pio, uint sm, Matriz_leds_config matriz)
{
    imprimir_desenho(matriz, pio, sm);
}

// Funções para imprimir os números
void n_zero(PIO pio, uint sm)
{
    Matriz_leds_config matriz_zero = {
        {RGB_OFF, RGB_BLUE, RGB_BLUE, RGB_BLUE, RGB_OFF}, // Linha 0
        {RGB_OFF, RGB_BLUE, RGB_OFF, RGB_BLUE, RGB_OFF}, // Linha 1
        {RGB_OFF, RGB_BLUE, RGB_OFF, RGB_BLUE, RGB_OFF}, // Linha 2
        {RGB_OFF, RGB_BLUE, RGB_OFF, RGB_BLUE, RGB_OFF}, // Linha 3
        {RGB_OFF, RGB_BLUE, RGB_BLUE, RGB_BLUE, RGB_OFF}, // Linha 4
    };
    imprimir_numero(pio, sm, matriz_zero);
}

void n_um(PIO pio, uint sm)
{
    Matriz_leds_config matriz_um = {
        {RGB_OFF, RGB_OFF, RGB_BLUE, RGB_OFF, RGB_OFF}, // Linha 0
        {RGB_OFF, RGB_BLUE, RGB_BLUE, RGB_OFF, RGB_OFF}, // Linha 1
        {RGB_OFF, RGB_OFF, RGB_BLUE, RGB_OFF, RGB_OFF}, // Linha 2
        {RGB_OFF, RGB_OFF, RGB_BLUE, RGB_OFF, RGB_OFF}, // Linha 3
        {RGB_OFF, RGB_BLUE, RGB_BLUE, RGB_BLUE, RGB_OFF}, // Linha 4
    };
    imprimir_numero(pio, sm, matriz_um);
}

void n_dois(PIO pio, uint sm)
{
    Matriz_leds_config matriz_dois = {
        {RGB_OFF, RGB_BLUE, RGB_BLUE, RGB_BLUE, RGB_OFF}, // Linha 0
        {RGB_OFF, RGB_OFF, RGB_OFF, RGB_BLUE, RGB_OFF}, // Linha 1
        {RGB_OFF, RGB_BLUE, RGB_BLUE, RGB_BLUE, RGB_OFF}, // Linha 2
        {RGB_OFF, RGB_BLUE, RGB_OFF, RGB_OFF, RGB_OFF}, // Linha 3
        {RGB_OFF, RGB_BLUE, RGB_BLUE, RGB_BLUE, RGB_OFF}, // Linha 4
    };
    imprimir_numero(pio, sm, matriz_dois);
}

void n_tres(PIO pio, uint sm)
{
    Matriz_leds_config matriz_tres = {
        {RGB_OFF, RGB_BLUE, RGB_BLUE, RGB_BLUE, RGB_OFF}, // Linha 0
        {RGB_OFF, RGB_OFF, RGB_OFF, RGB_BLUE, RGB_OFF}, // Linha 1
        {RGB_OFF, RGB_BLUE, RGB_BLUE, RGB_BLUE, RGB_OFF}, // Linha 2
        {RGB_OFF, RGB_OFF, RGB_OFF, RGB_BLUE, RGB_OFF}, // Linha 3
        {RGB_OFF, RGB_BLUE, RGB_BLUE, RGB_BLUE, RGB_OFF}, // Linha 4
    };
    imprimir_numero(pio, sm, matriz_tres);
}

void n_quatro(PIO pio, uint sm)
{
    Matriz_leds_config matriz_quatro = {
        {RGB_OFF, RGB_BLUE, RGB_OFF, RGB_BLUE, RGB_OFF}, // Linha 0
        {RGB_OFF, RGB_BLUE, RGB_OFF, RGB_BLUE, RGB_OFF}, // Linha 1
        {RGB_OFF, RGB_BLUE, RGB_BLUE, RGB_BLUE, RGB_OFF}, // Linha 2
        {RGB_OFF, RGB_OFF, RGB_OFF, RGB_BLUE, RGB_OFF}, // Linha 3
        {RGB_OFF, RGB_OFF, RGB_OFF, RGB_BLUE, RGB_OFF}, // Linha 4
    };
    imprimir_numero(pio, sm, matriz_quatro);
}

void n_cinco(PIO pio, uint sm)
{
    Matriz_leds_config matriz_cinco = {
        {RGB_OFF, RGB_BLUE, RGB_BLUE, RGB_BLUE, RGB_OFF}, // Linha 0
        {RGB_OFF, RGB_BLUE, RGB_OFF, RGB_OFF, RGB_OFF}, // Linha 1
        {RGB_OFF, RGB_BLUE, RGB_BLUE, RGB_BLUE, RGB_OFF}, // Linha 2
        {RGB_OFF, RGB_OFF, RGB_OFF, RGB_BLUE, RGB_OFF}, // Linha 3
        {RGB_OFF, RGB_BLUE, RGB_BLUE, RGB_BLUE, RGB_OFF}, // Linha 4
    };
    imprimir_numero(pio, sm, matriz_cinco);
}

void n_seis(PIO pio, uint sm)
{
    Matriz_leds_config matriz_seis = {
        {RGB_OFF, RGB_BLUE, RGB_BLUE, RGB_BLUE, RGB_OFF}, // Linha 0
        {RGB_OFF, RGB_BLUE, RGB_OFF, RGB_OFF, RGB_OFF}, // Linha 1
        {RGB_OFF, RGB_BLUE, RGB_BLUE, RGB_BLUE, RGB_OFF}, // Linha 2
        {RGB_OFF, RGB_BLUE, RGB_OFF, RGB_BLUE, RGB_OFF}, // Linha 3
        {RGB_OFF, RGB_BLUE, RGB_BLUE, RGB_BLUE, RGB_OFF}, // Linha 4
    };
    imprimir_numero(pio, sm, matriz_seis);
}

void n_sete(PIO pio, uint sm)
{
    Matriz_leds_config matriz_sete = {
        {RGB_OFF, RGB_BLUE, RGB_BLUE, RGB_BLUE, RGB_OFF}, // Linha 0
        {RGB_OFF, RGB_OFF, RGB_OFF, RGB_BLUE, RGB_OFF}, // Linha 1
        {RGB_OFF, RGB_OFF, RGB_OFF, RGB_BLUE, RGB_OFF}, // Linha 2
        {RGB_OFF, RGB_OFF, RGB_OFF, RGB_BLUE, RGB_OFF}, // Linha 3
        {RGB_OFF, RGB_OFF, RGB_OFF, RGB_BLUE, RGB_OFF}, // Linha 4
    };
    imprimir_numero(pio, sm, matriz_sete);
}

void n_oito(PIO pio, uint sm)
{
    Matriz_leds_config matriz_oito = {
        {RGB_OFF, RGB_BLUE, RGB_BLUE, RGB_BLUE, RGB_OFF}, // Linha 0
        {RGB_OFF, RGB_BLUE, RGB_OFF, RGB_BLUE, RGB_OFF}, // Linha 1
        {RGB_OFF, RGB_BLUE, RGB_BLUE, RGB_BLUE, RGB_OFF}, // Linha 2
        {RGB_OFF, RGB_BLUE, RGB_OFF, RGB_BLUE, RGB_OFF}, // Linha 3
        {RGB_OFF, RGB_BLUE, RGB_BLUE, RGB_BLUE, RGB_OFF}, // Linha 4
    };
    imprimir_numero(pio, sm, matriz_oito);
}

void n_nove(PIO pio, uint sm)
{
    Matriz_leds_config matriz_nove = {
        {RGB_OFF, RGB_BLUE, RGB_BLUE, RGB_BLUE, RGB_OFF}, // Linha 0
        {RGB_OFF, RGB_BLUE, RGB_OFF, RGB_BLUE, RGB_OFF}, // Linha 1
        {RGB_OFF, RGB_BLUE, RGB_BLUE, RGB_BLUE, RGB_OFF}, // Linha 2
        {RGB_OFF, RGB_OFF, RGB_OFF, RGB_BLUE, RGB_OFF}, // Linha 3
        {RGB_OFF, RGB_BLUE, RGB_BLUE, RGB_BLUE, RGB_OFF}, // Linha 4
    };
    imprimir_numero(pio, sm, matriz_nove);
}

int main()
{
    stdio_init_all();

    // Inicializando os botões
    configurar_botao(BUTTON_A_PIN);
    configurar_botao(BUTTON_B_PIN);
    configurar_botao(BUTTON_C_PIN);

    // Interrupção caso o botão for pressionado
    gpio_set_irq_enabled_with_callback(BUTTON_A_PIN, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
    gpio_set_irq_enabled_with_callback(BUTTON_B_PIN, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
    gpio_set_irq_enabled_with_callback(BUTTON_C_PIN, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);

    // Variáveis para controle da matriz
    PIO pio = pio0;
    uint sm = configurar_matriz(pio);
    
    while (true)
    {   
        piscar_led();   // Ativa a função de piscar o led

        if (modo==true) {desligar(pio, sm);} // Deliga os leds e entra em bootsel

        // Chamando as funções de acordo com o valor do número
        switch(numero) {
            case 1:
                n_um(pio, sm);
                break;
            case 2:
                n_dois(pio, sm);
                break;
            case 3:
                n_tres(pio, sm);
                break;
            case 4:
                n_quatro(pio, sm);
                break;
            case 5:
                n_cinco(pio, sm);
                break;
            case 6:
                n_seis(pio, sm);
                break;
            case 7:
                n_sete(pio, sm);
                break;
            case 8:
                n_oito(pio, sm);
                break;
            case 9:
                n_nove(pio, sm);
                break;
            case 0:
                n_zero(pio, sm);
                break;
        } 
    } 
}
