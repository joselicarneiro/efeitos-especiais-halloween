# 💀 Maria Joaquina — Olhos da Caveira

## O que é?

Maria Joaquina é a caveira cenográfica do Arthuween 2026.

Seus olhos foram construídos com:

- ESP32-S3 DevKit;
- 2 × displays circulares GC9A01;
- aproximadamente 1,28";
- resolução de 240 × 240;
- displays instalados atrás de óculos escuros.

Os óculos escondem as placas e os fios, fazendo com que o que apareça para o público seja simplesmente um par de olhos dentro da caveira.

## A ideia principal

Os olhos não são vídeos.

Eles são **gerados em tempo real**.

Cada olho é composto por círculos:

```
┌─────────────────────────┐
│                         │
│       ESCLERA           │
│          ●              │
│       ÍRIS +            │
│       PUPILA            │
│                         │
└─────────────────────────┘
```

A unidade que se movimenta é a **íris inteira**:

**anel externo + íris + pupila**

A pupila permanece centralizada em relação à íris.

Isso produz um movimento visual muito mais simples e coerente do que tentar movimentar cada elemento separadamente.

## Baseline congelado

A referência funcional é:

**POC-KAME-EYE-002C — IRIS GAZE + NUMB / LIFELESS**

Parâmetros validados:

| Parâmetro | Valor |
|---|---:|
| Centro X | 120 |
| Centro Y | 120 |
| Esclera | 105 |
| Anel externo | 65 |
| Íris | 55 |
| Pupila | 25 |
| Buffer | 201 × 201 |
| Buffer X | 20 |
| Buffer Y | 20 |

Posições de olhar:

- esquerda = 90
- centro = 120
- direita = 150
- cima = 90
- baixo = 150

O framebuffer utiliza RGB565 e é enviado aos dois displays com `pushImage()`.

## Como o movimento funciona

O movimento não pula de uma posição para outra.

O código usa uma curva de suavização:

```
t² × (3 - 2t)
```

Isso produz um movimento de entrada e saída suave.

A sequência normal inclui:

- centro → cima → centro;
- centro → baixo → centro;
- centro → esquerda → centro;
- centro → direita → centro;
- centro → cima-esquerda → centro;
- centro → baixo-direita → centro.

## NUMB / LIFELESS

De vez em quando, o olho entra em um estado diferente.

Nesse estado:

- a íris continua visível;
- o olhar fica lento/minimalista;
- a pupila fica cinza-clara;
- depois de alguns segundos o olho retorna ao comportamento normal.

No arquivo-base 002C:

- chance: 3%;
- duração: aproximadamente 2,5–5 segundos.

Durante os testes, 15% também foi experimentado, mas o arquivo-base congelado permanece com 3%.

## Uma decisão importante: autonomia

Foram considerados sensores HC-SR04 para detectar pessoas e alterar o comportamento dos olhos.

Essa ideia foi retirada do escopo do Arthuween.

Maria Joaquina funciona de maneira **autônoma**.

Os sensores ficaram disponíveis para experimentos futuros.

## Abordagens que foram descartadas

### Rastreamento de pessoas

Retirado do projeto final.

### Blink

Não utilizado porque a caveira não possui pálpebras.

### Animação por imagens

Rejeitada. O comportamento dos olhos deve permanecer procedural.

### LGFX_Sprite

Foi testado e provocou `StoreProhibited`. Não faz parte da solução validada.

### readRect()

Foi testado, mas o driver estava configurado com `readable=false`. Não foi utilizado como estratégia de framebuffer.

### Redesenhar a tela inteira

Provocava flicker durante o movimento.

A solução validada utiliza um framebuffer local e `pushImage()`.

## Um detalhe crítico do hardware

Os dois GC9A01 utilizam CS compartilhado no barramento SPI.

A inicialização validada do 002C deve ser preservada.

A sequência é:

1. CS esquerdo em LOW;
2. inicialização através de `rightDisplay.init()`;
3. rotação esquerda = 0;
4. rotação direita = 2;
5. desenho nos dois displays;
6. ambos os CS em HIGH.

**Não alterar essa sequência dentro da versão congelada.**

## Resultado

Maria Joaquina foi instalada no Arthuween 2026 e fez muito sucesso com o público.

A validação deixou de ser apenas técnica: o comportamento foi observado em uma instalação cenográfica real.

## Código

`POC-KAME-EYE-002C_IrisGaze_Numb_Lifeless.ino` é a cópia versionada do código-base funcional.

