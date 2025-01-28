# Contador_LED

# Contador de 1 a 9 com Matriz de LEDs

Este projeto implementa um contador de 1 a 9 utilizando uma matriz de LEDs controlada por um microcontrolador Raspberry Pi Pico. O contador pode ser incrementado ou decrementado através de botões, e um LED adicional pisca enquanto o contador está ativo. O projeto também inclui a funcionalidade de desligar a matriz de LEDs e entrar no modo de bootsel do Raspberry Pi Pico.

## Funcionalidades

- **Contador de 0 a 9**: O contador é exibido em uma matriz de LEDs 5x5.
- **Botões de controle**:
  - **Botão A**: Incrementa o contador.
  - **Botão B**: Decrementa o contador.
  - **Botão C**: Desliga os LEDs e entra no modo de bootsel.
- **LED indicador**: Um LED vermelho pisca enquanto o contador está ativo.

## Estrutura do Código

O código está organizado da seguinte forma:

- **Inclusão de bibliotecas**: Inclui as bibliotecas necessárias para controlar o Raspberry Pi Pico, a matriz de LEDs e os botões.
- **Definição de pinos**: Define os pinos utilizados para a matriz de LEDs, botões e LED indicador.
- **Funções para controle da matriz de LEDs**:
  - `gerar_binario_cor`: Converte valores de cor RGB em um formato binário para ser enviado à matriz de LEDs.
  - `configurar_matriz`: Configura a matriz de LEDs e o clock do sistema.
  - `imprimir_desenho`: Envia os dados para a matriz de LEDs, exibindo o desenho correspondente ao número atual.
- **Funções de interrupção**:
  - `gpio_irq_handler`: Gerencia as interrupções dos botões, atualizando o contador e o modo de operação.
- **Funções auxiliares**:
  - `piscar_led`: Faz o LED vermelho piscar.
  - `configurar_botao`: Configura os botões como entradas com pull-up.
  - `desligar`: Desliga a matriz de LEDs e entra no modo de bootsel.
- **Funções para exibir números**:
  - `n_zero` a `n_nove`: Funções que definem os padrões de LEDs para cada número de 0 a 9.
- **Função principal (`main`)**:
  - Inicializa os botões e a matriz de LEDs.
  - Entra em um loop infinito onde verifica o estado dos botões e exibe o número correspondente na matriz de LEDs.

## Como Usar

1. **Compilação**: Compile o código utilizando o ambiente de desenvolvimento apropriado para o Raspberry Pi Pico (por exemplo, o SDK do Raspberry Pi Pico).
2. **Upload**: Carregue o código compilado no Raspberry Pi Pico.
3. **Operação**:
   - Pressione o **Botão A** para incrementar o contador.
   - Pressione o **Botão B** para decrementar o contador.
   - Pressione o **Botão C** para desligar a matriz de LEDs e entrar no modo de bootsel.

## Requisitos

- **Hardware**:
  - Raspberry Pi Pico.
  - Matriz de LEDs 5x5.
  - Três botões.
  - LED vermelho.
  - Resistores e fios conforme necessário.
- **Software**:
  - SDK do Raspberry Pi Pico.
  - Compilador compatível com C/C++.