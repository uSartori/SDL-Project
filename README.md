
# Editor Gráfico 2D com SDL2 em C++

Aplicação desenvolvida em linguagem C++ utilizando a biblioteca `SDL2` para criação e manipulação de figuras geométricas bidimensionais.

O projeto foi desenvolvido para a disciplina de Computação Gráfica da Universidade de Caxias do Sul (UCS), com foco na implementação de algoritmos de rasterização, transformações geométricas e interação gráfica.

O sistema realiza:

- Desenho de linhas, retângulos e círculos
- Criação de curvas de Bézier
- Criação de polígonos
- Seleção e manipulação de figuras
- Translação, escala e rotação
- Preenchimento de regiões utilizando Flood Fill
- Alteração de cores
- Salvamento da imagem do canvas

---

## Funcionalidades

### Desenho de figuras geométricas

O sistema permite criar diferentes primitivas gráficas por meio da interação com o mouse:

- Linhas
- Retângulos
- Círculos
- Curvas de Bézier
- Polígonos

Cada tipo de figura possui sua própria implementação, responsável pelo desenho e pelas operações de transformação.

---

## Rasterização de figuras

O projeto implementa algoritmos de rasterização para representar figuras geométricas em uma superfície de pixels.

### Algoritmo de Bresenham

Utilizado para o desenho de círculos e como alternativa para rasterização de linhas.

O algoritmo determina quais pixels devem ser desenhados para representar a figura, utilizando cálculos eficientes com números inteiros.

### Algoritmo de Xiaolin Wu

Utilizado para desenhar linhas com suavização de bordas (*antialiasing*).

O algoritmo distribui intensidades entre pixels próximos à linha para reduzir o efeito de serrilhamento.

### Curvas de Bézier

O sistema utiliza curvas de Bézier cúbicas definidas por quatro pontos.

A curva é calculada a partir dos pontos de controle, permitindo representar formas curvas na área de desenho.

---

## Sistema de Seleção

A aplicação possui uma ferramenta de seleção que permite identificar e manipular figuras presentes no canvas.

O sistema utiliza:

- Verificação de proximidade entre cursor e figura
- Tolerância de seleção de aproximadamente 5 pixels
- Caixa delimitadora (*bounding box*)
- Controles visuais para transformação

### Critério de seleção

A seleção utiliza a distância euclidiana para verificar a proximidade do cursor em relação às figuras.

Esse método permite selecionar objetos mesmo quando o cursor não está exatamente sobre um de seus pixels.

---

## Transformações Geométricas

O projeto implementa três operações principais para manipulação das figuras:

### Translação

Deslocamento da figura no eixo X e Y.

A transformação é aplicada aos pontos que representam a figura.

### Escala

Alteração das dimensões da figura utilizando controles de transformação.

O sistema utiliza a caixa delimitadora como referência para o redimensionamento, considerando o ponto oposto ao controle selecionado.

### Rotação

Alteração da orientação da figura em torno de um ponto de referência.

O projeto utiliza o centro da caixa delimitadora como pivô de rotação.

As operações de transformação são organizadas na classe `Transform`.

---

## Sistema de Preenchimento

O projeto utiliza o algoritmo `Flood Fill` para preenchimento de regiões da área de desenho.

O algoritmo identifica a cor do pixel inicial e preenche os pixels conectados que correspondem à cor de referência.

### Funcionamento

- Seleção da posição inicial
- Identificação da cor de referência
- Verificação dos pixels vizinhos
- Preenchimento da região com a cor selecionada

O sistema mantém os preenchimentos para que possam ser reaplicados durante a renderização do canvas.

---

## Sistema de Cores

A aplicação disponibiliza ferramentas para seleção da cor utilizada no desenho.

O usuário pode escolher entre as cores disponíveis na barra de ferramentas.

A cor selecionada é utilizada nas operações de desenho e nos recursos compatíveis de preenchimento.

---

## Salvamento de Imagens

O projeto possui uma classe dedicada ao salvamento da imagem:

```
ImageSaver
```

O sistema permite salvar a imagem produzida no canvas.

### Características

- Captura da área de desenho
- Exclusão da barra de ferramentas da imagem salva
- Armazenamento da imagem em arquivo
- Separação entre interface e conteúdo do canvas

O caminho de salvamento depende da configuração utilizada pela aplicação.

```
// Windows
getenv("USERPROFILE")
```
```
// Linux
getenv("HOME")
```

---

## Estrutura do Projeto

O projeto utiliza Programação Orientada a Objetos para organização dos componentes.

### Principais classes

| Classe | Responsabilidade |
|---|---|
| `Context` | Gerenciamento do contexto gráfico SDL |
| `Canvas` | Área de desenho e gerenciamento das figuras |
| `Shape` | Classe base das figuras geométricas |
| `Point` | Representação de pontos bidimensionais |
| `Line` | Desenho de linhas |
| `Circle` | Desenho de círculos |
| `Rectangle` | Desenho de retângulos |
| `Polygon` | Desenho de polígonos |
| `Curve` | Desenho de curvas de Bézier |
| `Transform` | Operações de transformação geométrica |
| `Toolbar` | Gerenciamento da barra de ferramentas |
| `FloodFill` | Preenchimento de regiões |
| `ImageSaver` | Salvamento de imagens |

---

## Programação Orientada a Objetos

A aplicação utiliza uma classe base `Shape` para representar as diferentes figuras geométricas.

As classes específicas herdam da classe base e implementam seus próprios comportamentos.

O projeto utiliza:

- Classes e objetos
- Herança
- Polimorfismo
- Encapsulamento
- Separação de responsabilidades

Essa organização permite trabalhar com diferentes tipos de figuras por meio de uma estrutura comum.

---

## Tecnologias Utilizadas

- Linguagem C++
- Biblioteca SDL2
- Programação Orientada a Objetos
- Algoritmo de Bresenham
- Algoritmo de Xiaolin Wu
- Curvas de Bézier
- Algoritmo Flood Fill
- Code::Blocks
- Git e GitHub

---

## Requisitos

Para executar o projeto, é necessário possuir:

- Sistema operacional compatível com a configuração do projeto
- Compilador C++ com suporte às funcionalidades utilizadas
- Code::Blocks com MinGW
- Biblioteca SDL2 configurada
- Dependências de compilação e execução da SDL2

---

## Como instalar

Clone o repositório utilizando:

```bash
git clone https://github.com/uSartori/SDL-Project.git
```

Acesse a pasta do projeto:

```bash
cd SDL-Project
```

Abra o arquivo do projeto no Code::Blocks e verifique as configurações da biblioteca SDL2.

---

## Como compilar

Abra o projeto no Code::Blocks:

```txt
SDL_Project.cbp
```

Configure os diretórios de inclusão e bibliotecas da SDL2 conforme o ambiente instalado.

Depois, utilize a opção de compilação (*Build*) do Code::Blocks.

---

## Objetivos Acadêmicos

O projeto foi desenvolvido com o objetivo de aplicar conceitos fundamentais de Computação Gráfica, incluindo:

- Representação de objetos 2D
- Rasterização
- Transformações geométricas
- Curvas paramétricas
- Manipulação de pixels
- Interação gráfica
- Programação Orientada a Objetos

---

## Autores

Projeto desenvolvido por:

- Milena Baroni Gambetta
- Wellinton Sartori

Universidade de Caxias do Sul — UCS

Disciplina: Computação Gráfica

Trabalho de Implementação 1

---

## Observação

Projeto desenvolvido para fins acadêmicos na disciplina de Computação Gráfica da Universidade de Caxias do Sul.
