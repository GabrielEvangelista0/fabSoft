# 🏃 Fuja das Calorias! — Endless Runner 3D

[![C++17](https://img.shields.io/badge/Language-C%2B%2B17-blue.svg)](https://en.wikipedia.org/wiki/C%2B%2B17)
[![Raylib](https://img.shields.io/badge/Graphics-Raylib%205.5-orange.svg)](https://www.raylib.com/)
[![Build](https://img.shields.io/badge/Build-CMake-brightgreen.svg)](https://cmake.org/)
[![Tests](https://img.shields.io/badge/Tests-7%20Passed-success.svg)](tests/test_game.cpp)

Projeto prático desenvolvido para a disciplina de **Fábrica de Software** (20% da média final).  
Trata-se de um jogo eletrônico no estilo **Endless Runner 3D** (estilo *Subway Surfers*) focado em conscientização nutricional, hábitos saudáveis e sobrevivência.

---

## 🎯 Proposta do Jogo e Regras

O personagem corre automaticamente por uma pista de 3 raias infinitas. O objetivo do jogador é desviar de alimentos calóricos e ultraprocessados enquanto coleta alimentos saudáveis e ricos em nutrientes para controlar o peso corporal e sobreviver o maior tempo possível.

### 🍎 Alimentos Saudáveis (Verdes / Coletáveis)
- **Maçã Verde**: Reduz 8 kg, concede +100 pontos.
- **Cenoura Fresca**: Reduz 10 kg, concede +120 pontos.
- **Carne Grelhada**: Reduz 14 kg, concede +160 pontos.
- Efeito: Mantém o personagem em forma atlética, diminui o indicador de peso e acumula bônus de pontuação.

### 🍔 Alimentos Ruins / Ultraprocessados (Vermelhos / Obstáculos)
- **Hambúrguer Triplo**: Aumenta +18 kg.
- **Batata Frita**: Aumenta +14 kg.
- **Refrigerante Cola**: Aumenta +12 kg.
- **Donut Açucarado**: Aumenta +16 kg.
- Efeito: Aumenta o peso corporal. Conforme o peso sobe, o personagem passa de **Normal** (azul) para **Sobrepeso** (dourado/lento) e **Obeso** (vermelho/pesado). Ao atingir o limite de **100 kg**, ocorre **Game Over** por colapso cardíaco!

### ⚡ Obstáculos e Alturas
- **Obstáculos no Chão**: O jogador deve **PULAR** (`Espaço` ou `↑`) ou mudar de raia para desviar.
- **Obstáculos Suspensos no Ar**: O jogador deve **AGACHAR** (`S` ou `↓`) ou mudar de raia para passar por baixo.
- **Frutas Suspensas**: O jogador pode **PULAR** para alcançar itens especiais e obter pontuações altas.

---

## 🎮 Controles

| Tecla | Ação |
|---|---|
| `A` / `D` ou `←` / `→` | Trocar de raia (esquerda, centro, direita) com interpolação suave |
| `Espaço` ou `↑` | Pular com física e gravidade simulada |
| `S` ou `↓` | Agachar (reduz hitbox e escala vertical) |
| `P` | Pausar / Retomar a partida |
| `M` | Silenciar / Ativar efeitos sonoros |
| `Enter` | Iniciar jogo (no Menu) ou Reiniciar (no Game Over / Pausa) |
| `Esc` | Sair do jogo ou retornar ao Menu |

---

## 🏗️ Arquitetura e Boas Práticas

O projeto foi construído do zero seguindo rigorosas boas práticas de Engenharia e Fábrica de Software:

```
fabSoft/
├── CMakeLists.txt              # Configuração CMake e busca automática do Raylib 5.5
├── README.md                   # Documentação completa
├── PLANO_DE_IMPLEMENTACAO.md   # Registro do planejamento das 3 sprints
├── RELATORIO_FINAL_E_APRESENTACAO.md # Roteiro de apresentação e defesa do projeto
│
├── src/
│   ├── main.cpp                # Ponto de entrada da aplicação
│   ├── core/
│   │   ├── AudioManager.h/.cpp # Síntese sonora procedural em tempo real (PCM 16-bit)
│   │   ├── GameManager.h/.cpp  # Máquina de estados (Menu, Playing, Paused, GameOver)
│   │   └── Settings.h          # Constantes globais (dimensões, física, limites)
│   │
│   ├── entities/
│   │   ├── Player.h/.cpp       # Personagem corredor 3D animado com física e pesos
│   │   └── Food.h/.cpp         # Hierarquia polimórfica (Food -> GoodFood/BadFood -> Apple/Burger...)
│   │
│   ├── systems/
│   │   ├── DifficultyManager.h/.cpp # Progressão dinâmica de velocidade e probabilidade
│   │   ├── ScoreSystem.h/.cpp       # Distância, multiplicadores e contadores de itens
│   │   └── SaveSystem.h/.cpp        # Persistência de recorde local em highscore.txt
│   │
│   ├── ui/
│   │   └── HUD.h/.cpp          # Renderização de HUD, Barra de Peso, Menus e Telas
│   │
│   └── world/
│       ├── TrackSpawner.h/.cpp # Pista infinita reciclada, calçadas, árvores, postes e horizonte
│       └── FoodSpawner.h/.cpp  # Object Pooling com std::unique_ptr e spawn 3D dinâmico
│
└── tests/
    └── test_game.cpp           # Suíte de 7 testes unitários automatizados (CTest)
```

### Padrões e Conceitos C++ Aplicados:
1. **State Machine:** Estados de jogo estruturados em `enum class GameState` no `GameManager`.
2. **Herança e Polimorfismo:** Classe base abstrata `Food` com subclasses `GoodFood` e `BadFood` e especializações concretas (`Apple`, `Carrot`, `Burger`, `SodaCan`, etc.).
3. **Object Pooling:** Gerenciador de comidas reutiliza instâncias ativas/inativas sem alocações no loop principal.
4. **Ponteiros Inteligentes:** `std::unique_ptr` em coleções polimórficas (`std::vector<std::unique_ptr<Food>>`), eliminando vazamentos de memória.
5. **Síntese Sonora Procedural:** Módulo `AudioManager` gera ondas sonoras PCM em memória (saltos, moedas, colisões, derrota), dispensando arquivos externos.
6. **Persistência RAII:** Leitura e gravação de `highscore.txt` com tratamento de exceções e integridade.
7. **Testes Automatizados:** 7 suítes completas de testes unitários integradas ao CTest.

---

## 🛠️ Como Compilar e Executar

### Pré-requisitos
- **CMake** 3.16 ou superior
- Compilador C++17 (MSVC no Windows, GCC ou Clang no Linux/macOS)
- Conexão à internet apenas na primeira configuração (o CMake baixa o Raylib 5.5 automaticamente)

### Compilação (Windows / PowerShell)

```powershell
# 1. Gerar arquivos de build
cmake -S . -B build

# 2. Compilar em modo Release
cmake --build build --config Release

# 3. Executar o jogo
.\build\Release\FujaDasCalorias.exe
```

### Executar a Suíte de Testes

```powershell
# Executar o binário de testes diretamente
.\build\Release\FujaDasCaloriasTests.exe

# Ou via CTest
ctest --test-dir build -C Release --output-on-failure
```

---

## 📅 Entregas Incrementais (Sprints)

- **Sprint 1 (Protótipo Jogável):** Janela 3D, câmera em perspectiva, pista infinita com 3 raias, player com física de salto e troca suave de raias, colisão AABB básica.
- **Sprint 2 (Mecânica e Dificuldade):** Mecânica completa de peso/saúde, transformação visual do player (Normal / Sobrepeso / Obeso), HUD dinâmico com barra gradiente de peso, sistema de dificuldade progressiva (velocidade e spawn por distância), persistência de recorde local.
- **Sprint 3 (Polimento, Arte, Áudio e Testes):** Modelagem 3D procedural de comidas (hambúrguer, maçã, batata frita, etc.), pista detalhada com árvores, calçadas e postes, sintetizador de áudio procedural (sem dependência de assets externos), sistema de pausa e popups de pontos, 7 suítes de testes unitários validadas no CTest.
- **Entrega Final:** Código 100% autoral, compilado e testado, executável Release pronto, documentação e roteiro de apresentação em sala.
