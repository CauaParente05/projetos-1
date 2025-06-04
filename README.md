# ⭐ Caça as Estrelas - Jogo Figital

## 📋 Sobre o Projeto

O Caça as Estrelas é um projeto desenvolvido para a disciplina de Projetos 1 da CESAR School. Este projeto tem com **intenção desenvolver um jogo figital com o objetivo de incluir pessoas com deficência**, o nosso projeto visa incluir pessoas com deficência motora. 

De acordo com o Relatório Mundial sobre Deficiência, cerca de **15% da população** vive com algum tipo de deficiência, **sendo 2%-4% portador de algum tipo de deficiência motora**. **1 a cada 10 pessoas** tem algum tipo de deficiência motora seja ela: monoplegia, hemiplegia, paraplegia, tetraplegia ou amputação. 

Com isso em mente criamos um jogo oferecendo funcionalidades direcionadas a elas após estudar suas dificuldades como a falta de equipamentos adequados, além da **falta de visibilidade no desenvolvimentos pela a insegurança em lidar com as necessidades específicas desses jogadores**. Com isso em mente criamos o Caça as estrelas para não só ser um jogo diverto voltado para a **maior parte das diferentes deficiências motoras** como também **proporciona saúde** usando o sopro como ferramenta para jogar, algo muito presente em **fisioterapias de pessoas que possuem deficiência motora**.

## 🛠️ Tecnologias Utilizadas

<div>
<img height="35" src="https://cdn.jsdelivr.net/gh/devicons/devicon@latest/icons/cplusplus/cplusplus-original.svg" />
<img height= "30" src="https://img.shields.io/badge/Arduino-00979D?style=for-the-badge&logo=arduino&logoColor=white"
</div>

### 📚 Bibliotecas Utilizadas:

Para desenvolver este projeto foram usadas as seguintes bibliotecas:

```C++
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_BMP280.h>
```

Peças usadas no prototipo:

1. 1 tela LCD I2C 16x2

2. 3 sensores de pressão I2C BMP 280 HW-611

3. 6 Leds RGBs

4. 1 sensor ultrassônico

5. 1 multiplexador TCA9548A I2C

<details>

<details>

<summary>📦 Estrutura do Projeto </summary>

- Acabamento todo feito de MDF pintado para parecer um céu estrelado

- Possui um braço articulado com espaço para os três sensores de pressão e o sensor ultrassônico

</details>

<details>

<summary>🚀 Como Jogar:</summary>

1. O jogo inciará ao se aproximar do sensor ultrassônico

2. A tela irá acender mostrando o tempo e a pontuação

3. As leds RGB acenderão em intervalos diferentes piscando alternadamente

4. Assopre os sensores com as cores de acordo com a luz que acendeu para marcar pontos

5. Após o tempo acabar a tela mostrará o total de pontos marcados e irá dar um intevalo para reiniciar o jogo

</details>

<details>

<summary>🧠 Disciplina:</summary>

Este projeto foi desenvolvido para a disciplina PROJETOS 1

Orientador da disciplina: Diocleciano Dantas Neto 

</details>

<details>

<summary>👥 Membros do Grupo:</summary>

### Alunos
- Cauã Henrique Cardozo Carneiro Parente    CC | Turma A

- Lucas Antonio de Paula    Design | Turma A

- Joao Arthur Gomes Duarte  CC | Turma A

- Mateus Dornellas Camara de Freitas    CC | Turma A

- Mariana Maliu da Rocha Montarroyos    CC | Turma A

- Pãmala Aurea Dourado Oliveira Siqueira    Design | Turma A

- Danilo Boa Vista de Freitas Fonseca   Design | Turma A

- Raul Maia Barbosa     CC | Turma A

- Vitória Gabrielle Janeiro de Albuquerque  CC | Turma A

</details>
