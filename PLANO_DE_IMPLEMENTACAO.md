# 🏃 Plano de Implementação — Endless Runner 3D: Fuja das Calorias!

> **Disciplina:** Fabricação de Software (20% da média final)  
> **Tipo de Projeto:** Jogo Endless Runner 3D com 3 raias (estilo Subway Surfers)  
> **Linguagem:** C++17 + Raylib 5.5  
> **Status Geral:** ✅ 100% Concluído e Validado  

---

## 1. Visão Geral do Produto

**Nome do Jogo:** *Fuja das Calorias!*

**Conceito:** Um jogo endless runner **3D** com **3 raias** (estilo Subway Surfers) em que o personagem corre automaticamente por um cenário infinito. O jogador troca de raia, pula e agacha para **evitar comidas ruins** (ultraprocessadas e calóricas) e **coletar comidas saudáveis** (frutas, verduras, carnes grelhadas). O indicador de **peso/saúde** do personagem aumenta ao colidir com comidas ruins e diminui ao coletar as boas; quando o peso máximo (100 kg) é atingido, o jogo termina por colapso cardíaco.

---

## 2. Tecnologia Escolhida

| Item | Escolha | Justificativa |
|---|---|---|
| **Linguagem** | **C++17** | Padrão industrial, alta performance, controle de memória e forte aprendizado acadêmico |
| **Biblioteca Gráfica & Áudio** | **Raylib 5.5** | Suporte nativo a 3D, câmera em perspectiva, colisões, miniaudio integrado e UI rápida sem overhead |
| **Build System** | **CMake** (3.16+) | Padrão da indústria para compilação multiplataforma e gestão de dependências via FetchContent |
| **Áudio** | Síntese Procedural PCM (16-bit) | Portabilidade total: sons gerados matematicamente em memória sem depender de arquivos externos |
| **Versionamento** | Git + GitHub | Controle de versão por sprints com histórico semântico |
| **Testes Unitários** | CTest + Asserções Customizadas | 7 suítes completas testando regras de negócio sem necessidade de janela gráfica |

---

## 3. Arquitetura do Projeto (C++ + Raylib)

```
fabSoft/
├── src/
│   ├── main.cpp                # Ponto de entrada
│   ├── core/
│   │   ├── AudioManager.h/.cpp # Síntese procedural de efeitos sonoros
│   │   ├── GameManager.h/.cpp  # Máquina de estados (Menu, Playing, Paused, GameOver)
│   │   └── Settings.h          # Constantes de configuração e física
│   ├── entities/
│   │   ├── Player.h/.cpp       # Personagem corredor 3D animado com física e pesos
│   │   └── Food.h/.cpp         # Hierarquia polimórfica de comidas 3D
│   ├── systems/
│   │   ├── DifficultyManager.h/.cpp # Progressão dinâmica de dificuldade
│   │   ├── SaveSystem.h/.cpp        # Persistência de recorde local
│   │   └── ScoreSystem.h/.cpp       # Pontuação, distância e estatísticas
│   ├── ui/
│   │   └── HUD.h/.cpp          # Renderização de HUD, Barra de Peso e Menus
│   └── world/
│       ├── TrackSpawner.h/.cpp # Pista infinita reciclada com cenário 3D
│       └── FoodSpawner.h/.cpp  # Object Pooling com std::unique_ptr
└── tests/
    └── test_game.cpp           # Suíte de testes unitários automatizados
```

---

## 4. Status das Sprints

### ✅ Sprint 1 — Protótipo Jogável Básico
**Objetivo:** Personagem 3D se move nas 3 raias, câmera funciona, colisão detectada.
- [x] Configurar `CMakeLists.txt` com FetchContent para Raylib 5.5
- [x] Criar janela 3D com câmera em perspectiva e acompanhamento suave
- [x] Implementar `Settings.h` com constantes globais
- [x] Implementar `Player` com troca de raia suave (`Lerp`), pulo com gravidade e agachamento
- [x] Implementar `TrackSpawner` — pista infinita com reciclagem de segmentos (Object Pooling)
- [x] Implementar `FoodSpawner` básico nas 3 raias
- [x] Implementar detecção de colisão AABB via `BoundingBox` e log no console
- [x] Histórico de commits no Git por sprint

---

### ✅ Sprint 2 — Mecânica Completa + Dificuldade Progressiva
**Objetivo:** Todas as mecânicas funcionais, balanceadas e interativas.
- [x] Implementar hierarquia OOP de `Food`, `GoodFood` e `BadFood`
- [x] Integrar mecânica de peso/saúde ao `Player` (reduz com comida boa, aumenta com comida ruim)
- [x] Implementar barra colorida de peso/saúde no HUD (Verde → Dourado → Vermelho)
- [x] Transformação visual do player conforme faixas de peso (Normal, Sobrepeso, Obeso)
- [x] Feedback visual de colisão com flash vermelho na tela
- [x] Implementar `ScoreSystem` com pontuação por distância e bônus de coleta
- [x] Implementar `DifficultyManager` com aumento de velocidade e frequência de obstáculos
- [x] Implementar State Machine no `GameManager` (`Menu`, `Playing`, `Paused`, `GameOver`)
- [x] Implementar tela de fim de jogo com estatísticas detalhadas da corrida
- [x] Implementar `SaveSystem` com gravação e leitura de recordes em `highscore.txt`
- [x] Gestão segura de memória com `std::vector` e `std::unique_ptr`

---

### ✅ Sprint 3 — Polimento, Arte 3D, Áudio Procedural e Testes
**Objetivo:** Jogo completo, polido, sonorizado, testado e pronto para apresentação.
- [x] Menu inicial completo com instruções e controles
- [x] Tela de Game Over detalhada com causa da morte e estatísticas
- [x] Modelagem 3D procedural de alimentos (Maçã, Cenoura, Bife Grelhado, Hambúrguer, Batatas Fritas, Refrigerante, Donut)
- [x] Rotação e animação 3D dos alimentos flutuando nas raias
- [x] Obstáculos em alturas diferenciadas (no chão exigindo pulo; suspensos exigindo agachamento)
- [x] Personagem humanoide estilizado com animação de corrida (membros alternados), pulo e agachamento
- [x] Cenário 3D rico: calçadas de concreto, meio-fio zebrado, árvores e postes de iluminação ao longo da pista, montanhas no horizonte e sol
- [x] Módulo `AudioManager` com síntese sonora procedural PCM 16-bit em memória (sem arquivos externos)
- [x] Efeitos de partículas ao coletar itens saudáveis
- [x] Popups flutuantes de pontuação e ganho/perda de peso no momento da colisão
- [x] Sistema de pausa (`P`) e controle de áudio mudo (`M`)
- [x] Criação de suíte de 7 testes unitários automatizados integrados ao CTest
- [x] Documentação e docstrings completas em todos os headers
- [x] Atualização completa do `README.md`
- [x] Compilação do binário final em modo Release (`build/Release/FujaDasCalorias.exe`)

---

### 🏁 Entrega Final
- [x] Repositório Git local inicializado com commits semânticos por sprint
- [x] `README.md` e roteiro de apresentação em sala (`RELATORIO_FINAL_E_APRESENTACAO.md`)
- [x] Executável testado e validado em `build/Release/`
- [x] Suíte de testes automatizados com 100% de aprovação (7/7)
