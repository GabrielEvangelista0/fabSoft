# 🎤 Roteiro de Apresentação e Relatório Final — Fuja das Calorias!

> **Disciplina:** Fábrica de Software (20% da média final)  
> **Tema:** Endless Runner 3D — Conscientização Nutricional e Sobrevivência  
> **Tecnologia:** C++17 + Raylib 5.5 + CMake  

Este documento foi elaborado para guiar a apresentação em sala de aula, demonstrar a evolução pelas sprints e preparar o grupo para eventuais perguntas técnicas do professor e da banca.

---

## 1. Mapeamento de Requisitos do Professor

Abaixo está a correspondência exata entre os requisitos obrigatórios da disciplina e as implementações realizadas:

| Requisito do Professor | Implementação no Jogo | Arquivo / Módulo |
|---|---|---|
| **Estilo Endless Runner (corrida automática)** | Pista infinita com 3 raias móveis e obstáculos dinâmicos avançando contra o jogador | `TrackSpawner.cpp`, `GameManager.cpp` |
| **Comidas ruins aumentam peso até morrer** | Fast-foods aumentam de 12 a 18 kg; ao atingir 100 kg ocorre Game Over por colapso cardíaco | `Food.cpp`, `Player.cpp`, `GameManager.cpp` |
| **Comidas boas reduzem peso e dão pontos** | Frutas e carnes reduzem 8 a 14 kg e dão bônus de 100 a 160 pontos | `Food.cpp`, `ScoreSystem.cpp` |
| **Indicador visível de peso/saúde na tela** | Barra gradiente (Verde → Dourado → Vermelho) com valor numérico (ex.: `25.0 / 100 kg`) e classificação (Fit / Sobrepeso / Obeso) | `HUD.cpp` |
| **Dificuldade progressiva** | Velocidade do mundo aumenta de 8 para até 17 m/s e taxa de comidas ruins sobe de 20% para 80% | `DifficultyManager.cpp` |
| **Telas de Menu e Fim de Jogo com pontos** | Menu inicial com tutorial e recorde; tela de Game Over com causa da morte e relatório completo de itens | `HUD.cpp`, `GameManager.cpp` |
| **Pulo e agachamento para desviar** | Obstáculos baixos exigem pulo (`Espaço`/`↑`); obstáculos altos exigem agachamento (`S`/`↓`) | `Player.cpp`, `FoodSpawner.cpp` |
| **Código autoral e boas práticas** | Padrões State Machine, OOP com Herança, Polimorfismo, Smart Pointers (`std::unique_ptr`), Object Pooling, RAII | Todo o código sob `src/` |
| **Entregas incrementais (Sprints)** | Sprint 1 (Protótipo), Sprint 2 (Mecânicas e Dificuldade), Sprint 3 (Arte 3D, Áudio, Testes) | `PLANO_DE_IMPLEMENTACAO.md` e histórico Git |
| **Testes de Software** | 7 suítes completas de testes unitários validadas no CTest | `tests/test_game.cpp` |

---

## 2. Roteiro Sugerido para a Apresentação (5 a 10 minutos)

### Parte 1: Introdução & Conceito (1 a 2 min)
- **Abertura:** Apresentar os integrantes do grupo e o nome do projeto: *Fuja das Calorias!*.
- **Problema & Conscientização:** Explicar a proposta lúdica — conscientizar sobre os perigos da alimentação ultraprocessada e a importância de escolhas saudáveis através de mecânicas clássicas de *Endless Runner* (estilo *Subway Surfers*).
- **Justificativa Tecnológica:** Por que C++ e Raylib?
  - C++17 oferece performance máxima, controle rigoroso de recursos e tipagem estática robusta.
  - O Raylib permite criar jogos 3D leves sem o overhead ou complexidade desnecessária de engines pesadas como Unreal, mantendo o foco nos conceitos de algoritmos, ponteiros inteligentes e arquitetura.

### Parte 2: Demonstração Prática ao Vivo (3 a 4 min)
1. **Menu Inicial:** Mostrar o design do menu com as instruções claras de jogo e o recorde salvo.
2. **Início da Partida:**
   - Iniciar com `ENTER`.
   - Mostrar a corrida automática nas 3 raias e a troca suave de raias (`A` e `D`).
   - Coletar uma maçã ou cenoura: mostrar a redução de peso na barra superior, os pontos subindo, o som de bônus e as partículas verdes.
   - Mostrar o pulo (`Espaço`) para desviar de um obstáculo no chão ou pegar fruta no ar.
   - Mostrar o agachamento (`S`) para passar por baixo de um hambúrguer suspenso.
3. **Mecânica de Peso:**
   - Deixar o personagem colidir com comidas calóricas propositalmente.
   - Mostrar o flash vermelho na tela e o som de impacto.
   - Mostrar a transformação visual do personagem: ele engorda visivelmente (barriga saliente, escala maior) e a camiseta muda de cor (Azul → Dourado → Vermelho).
   - Mostrar a barra de peso no HUD atingindo a zona crítica.
4. **Game Over:**
   - Atingir 100 kg. Mostrar a tela de fim de jogo com estatísticas: pontos, distância percorrida, quantas comidas saudáveis foram coletadas e quantos fast-foods foram ingeridos.
   - Destacar o som dramático de Game Over e o salvamento automático do novo recorde.

### Parte 3: Destaques de Arquitetura e Engenharia de Software (2 min)
- **Herança e Polimorfismo:** Explicar como a classe abstrata `Food` permite adicionar novos alimentos facilmente sem alterar o loop principal do jogo (Open/Closed Principle do SOLID).
- **Object Pooling:** Explicar que a pista e as comidas não são criadas e destruídas a todo momento com `new`/`delete`; elas são recicladas com `std::unique_ptr`, garantindo zero engasgos (sem coleta de lixo) e 60 FPS cravados.
- **Síntese de Áudio Procedural:** Destacar que o jogo não depende de nenhum arquivo `.wav` ou `.mp3` externo; todos os sons (pulo, itens, batida, derrota) são sintetizados matematicamente via PCM 16-bit pelo `AudioManager`.
- **Testes Automatizados:** Citar que foram criados 7 testes unitários automatizados cobrindo todas as regras de negócio essenciais.

---

## 3. Guia de Perguntas Frequentes da Banca (Como Defender)

### Pergunta 1: "Como vocês garantiram que o jogo não tem vazamentos de memória (memory leaks)?"
> **Resposta recomendada:**  
> *"Utilizamos o idioma RAII do C++ moderno. Todas as alocações dinâmicas de entidades polimórficas são encapsuladas em `std::unique_ptr`, que desalocam automaticamente os objetos quando saem de escopo. Além disso, utilizamos Object Pooling para as comidas e segmentos de pista, evitando alocações contínuas durante a partida."*

### Pergunta 2: "Como funciona a detecção de colisão em 3D?"
> **Resposta recomendada:**  
> *"Utilizamos colisão por AABB (Axis-Aligned Bounding Box) através de `BoundingBox` e da função `CheckCollisionBoxes` do Raylib. A caixa de colisão do jogador se adapta dinamicamente à sua escala de peso e ao estado de agachamento: quando ele agacha, a altura da caixa é reduzida para cerca de 35%, permitindo passar por baixo de comidas suspensas sem registrar colisão."*

### Pergunta 3: "Onde o áudio está armazenado se não há arquivos de som no projeto?"
> **Resposta recomendada:**  
> *"Desenvolvemos um módulo próprio chamado `AudioManager` que sintetiza formas de onda senoidais, ruído e ondas quadradas diretamente em buffers de memória PCM 16-bit a 44.1 kHz. Esses dados são carregados como `Sound` nativo pelo Raylib através de `LoadSoundFromWave`. Isso confere ao projeto 100% de portabilidade e independência de assets externos."*

### Pergunta 4: "Como a dificuldade aumenta ao longo do jogo?"
> **Resposta recomendada:**  
> *"A classe `DifficultyManager` calcula a velocidade do mundo e a probabilidade de obstáculos ruins com base na distância percorrida pelo jogador. A velocidade varia em faixas de 8 m/s até 17 m/s, e a chance de spawn de comidas não saudáveis progride de 20% até 80%, tornando a sobrevivência cada vez mais desafiadora."*

---

## 4. Como Subir o Projeto no GitHub (Para Entrega do Link)

No terminal PowerShell na raiz do projeto (`c:\Users\gbert\Desktop\fabSoft`):

```powershell
# 1. Crie um novo repositório vazio no GitHub (ex.: 'fuja-das-calorias')
# 2. Conecte seu repositório remoto:
git remote add origin https://github.com/SEU_USUARIO/SEU_REPOSITORIO.git

# 3. Defina a branch principal como main e envie os commits:
git branch -M main
git push -u origin main
```

Após o envio, o link do repositório estará pronto para entrega no portal da disciplina.
