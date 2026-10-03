# ⚡ Lightning Engine

## O que é?

O Lightning Engine é um simulador de tempestade para fitas de LEDs endereçáveis WS2812B.

A ideia central foi simples:

> Não queremos fazer LEDs piscarem. Queremos fazer uma tempestade parecer viva.

Por isso, o sistema possui uma noção de **tempo**, **intensidade**, **desenvolvimento da tempestade** e **personalidade de cada descarga**.

## Hardware validado

- Arduino Uno
- 2 × WS2812B, 60 LEDs cada
- 120 LEDs no total
- D7 → fita A
- D8 → fita B
- Fonte de 12 V / 3,33 A
- LM2596S ajustado para aproximadamente 5,02 V
- Fitas instaladas aproximadamente a 2,8 m de altura
- Algodão/enchimento usado como material difusor para formar a nuvem

As duas fitas são tratadas como entidades independentes. Isso permite criar ramificações e fazer a descarga passar de uma região lógica para outra.

## Como a tempestade funciona

A tempestade possui estados:

```
SILENCE
   ↓
RISING
   ↓
DEVELOPING
   ↓
PEAK
   ↓
DECAYING
   ↓
DYING
   ↓
SILENCE
```

O estado **SILENCE** é proposital.

Uma tempestade que pisca o tempo inteiro deixa de parecer uma tempestade e passa a parecer um efeito de iluminação.

## Cada relâmpago tem personalidade

Uma descarga pode variar em:

- intensidade;
- duração;
- tempo de propagação;
- direção;
- ramificação;
- persistência;
- curva de intensidade;
- número de picos.

As curvas utilizadas incluem:

- **Explosive** — começa forte e desaparece;
- **Growing** — cresce progressivamente;
- **Peaked** — cresce, chega a um pico e decai;
- **Multi-peak** — possui mais de um pico.

Nas tempestades fortes, a distribuição utilizada foi:

- 18% explosiva;
- 14% crescente;
- 44% com pico;
- 24% multi-pico.

## Propagação

Foram testados vários tempos entre pontos sucessivos da descarga:

| Propagação | Resultado observado |
|---:|---|
| 35/25 ms | discreto |
| 10 ms | flash |
| 5 ms | muito bom |
| 0 ms | intenso |
| 0–10 ms aleatório | validado |

A escolha aleatória entre 0 e 10 ms ajudou a evitar que todas as descargas apresentassem exatamente o mesmo comportamento.

## Potência

O limite teórico de 120 LEDs em branco a 60 mA seria:

`120 × 60 mA = 7,2 A`

Isso é superior à capacidade do conjunto de alimentação utilizado.

Por isso, o firmware trabalha com:

- efeitos esparsos;
- brilho variável;
- limite de potência;
- flashes localizados.

O limite de software usado no firmware é de aproximadamente **2400 mA**, equivalente a cerca de 40 LEDs em branco total.

### Alimentação

```
12 V / 3,33 A
       │
       ▼
    LM2596S
       │
     5,02 V
       │
       ├── Arduino Uno 5V
       │
       └── VCC das duas WS2812B

GND comum entre Arduino e fitas
```

**Importante:** os 5 V regulados são aplicados ao pino 5V do Uno, não ao barrel/DC jack.

## Resultado real

O sistema foi instalado em uma nuvem cenográfica e funcionou durante o evento Arthuween 2026.

O teste deixou de ser apenas de bancada: houve validação em ambiente real, diante de público.

Resultado relatado:

**mais de 25.000 CPM.**

## O que aprendemos

### 1. O difusor importa

A distância entre os LEDs e o material difusor influencia muito a sobreposição das emissões.

O enchimento de algodão/tecido funcionou muito bem.

### 2. A aleatoriedade precisa de estrutura

Aleatoriedade pura não produz necessariamente um fenômeno convincente.

O sistema usa aleatoriedade dentro de regras: estado da tempestade, intensidade, curvas, intervalos e probabilidade de ramificação.

### 3. O silêncio é parte do efeito

Talvez essa tenha sido uma das descobertas mais importantes.

A ausência de relâmpagos também comunica que existe uma tempestade acontecendo.

## Código

A versão registrada neste repositório é:

`Lightning_Simulator_v0_3_Tempestades_Intensas.ino`

Ela preserva a versão que contém o comportamento de tempestades intensas utilizado durante os testes finais.

---

### 🤖 Human Designed. AI Assisted.

This project was designed and developed by humans with the assistance of artificial intelligence. Technical decisions, experimentation, validation and final implementation remained under human direction.
