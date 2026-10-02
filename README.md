# 🎃 Efeitos Especiais para Festas de Halloween

Projetos eletrônicos desenvolvidos e testados para criar efeitos especiais de Halloween com hardware acessível, software procedural e muita experimentação.

Este repositório registra parte do trabalho realizado no **Arthuween 2026**, incluindo duas experiências principais:

- ⚡ **Lightning Engine** — uma tempestade de relâmpagos usando Arduino Uno e fitas WS2812B.
- 💀 **Maria Joaquina** — uma caveira cenográfica com dois olhos animados usando ESP32-S3 e displays GC9A01.

O objetivo não é apenas disponibilizar código. A documentação registra também as ideias, decisões, testes, problemas encontrados e versões que foram consideradas válidas.

## ⚡ Lightning Engine

A máquina de relâmpagos foi concebida como uma **tempestade procedural autônoma**, e não como um simples pisca-pisca.

Ela trabalha com:

- 2 Arduino Uno;
- 2 fitas WS2812B de 60 LEDs;
- 120 LEDs no total;
- D7 para a fita A e D8 para a fita B;
- fonte de 12 V / 3,33 A;
- LM2596S ajustado para aproximadamente 5,02 V;
- caminhos lógicos separados da posição física dos LEDs;
- intensidade, duração, propagação, ramificação e múltiplos picos;
- ciclo de vida da tempestade: `SILENCE → RISING → DEVELOPING → PEAK → DECAYING → DYING`.

Um detalhe importante do projeto é que **o silêncio também faz parte do efeito**. Uma tempestade convincente não deve produzir relâmpagos continuamente.

No Arthuween 2026, o sistema foi usado em uma nuvem cenográfica e ultrapassou **25.000 CPM (Caralhos Por Minuto)**, segundo o relato após o evento.

➡️ [Documentação do Lightning Engine](relampagos/README.md)

## 💀 Maria Joaquina — olhos da caveira

A caveira recebeu dois displays circulares GC9A01 de aproximadamente 1,28", controlados por um ESP32-S3.

Os olhos são construídos **proceduralmente**. Não são animações feitas com uma sequência de imagens.

A composição visual é formada por:

- esclera;
- anel externo;
- íris;
- pupila.

Durante o movimento, a unidade que se desloca é a **íris inteira** — anel + íris + pupila. A pupila permanece centralizada dentro da íris.

A versão funcional de referência é o:

**POC-KAME-EYE-002C — IRIS GAZE + NUMB / LIFELESS**

Essa versão é mantida como baseline congelado porque foi a última versão comprovadamente funcional com os dois olhos.

➡️ [Documentação dos olhos](olhos-caveira/README.md)

## 🧪 Como o projeto foi desenvolvido

O processo adotado foi deliberadamente experimental:

`POC → teste visual → validação técnica → montagem → evento real → reação humana → validação da experiência`

Versões validadas não devem ser alteradas diretamente. Uma alteração deve gerar uma nova versão.

## 🏠 Arthuween 2026

Além dos dois sistemas eletrônicos, a cenografia utilizou projeções em loop de fantasmas, zumbis e outros conteúdos sobrenaturais em uma janela voltada para o corredor lateral. As luzes do corredor foram apagadas para melhorar o contraste.

O resultado combinou:

**relâmpagos + Maria Joaquina + projeções + escuridão + surpresa + comportamento procedural.**

## 📚 Estrutura

```
relampagos/
  README.md
  codigo/
    Lightning_Simulator_v0_3_Tempestades_Intensas.ino

olhos-caveira/
  README.md
  codigo/
    POC-KAME-EYE-002C_IrisGaze_Numb_Lifeless.ino

docs/
  ARTHUWEEN-2026.md
```

## ⚠️ Observações

Os códigos são registros de versões de desenvolvimento e validação de um projeto cenográfico. Componentes, alimentação, corrente e montagem física devem ser dimensionados e verificados antes de qualquer reprodução.

## 🎃 Resultado

O Arthuween deixou de ser apenas uma coleção de POCs e se tornou um experimento completo envolvendo **engenharia, cenografia, software, hardware, comportamento procedural e experiência humana**.
