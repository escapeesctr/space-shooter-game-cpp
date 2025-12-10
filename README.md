# 🚀 Space Shooter Game - Project 13/20

A 2D space shooter game developed with C++ and SFML, featuring enemy AI, particle effects, power-ups, and wave-based progression.

## 🎮 Features

### Core Gameplay
- **Player Controls**: WASD/Arrow keys for movement, SPACE for shooting
- **Enemy AI**: Smart enemy movement and shooting patterns
- **Wave System**: Increasing difficulty with each wave
- **Boss Battles**: Special boss enemy every 3 waves
- **Power-ups**: Health, speed, and damage bonuses
- **Scoring System**: Points for destroying enemies

### Visual Effects
- **Particle Systems**: Explosion and trail effects
- **Dynamic UI**: Health bars, score display, wave counter
- **Background**: Starfield animation
- **Visual Feedback**: Damage indicators, power-up effects

### Game States
- **Main Menu**: Start game and exit options
- **Gameplay**: Core shooting action
- **Pause Menu**: Resume gameplay
- **Game Over**: Final score and restart option

## 🛠️ Technologies Used

- **C++17**: Modern C++ features and OOP design
- **SFML 2.6.2**: Graphics, window management, and input handling
- **Object-Oriented Design**: Clean class architecture
- **Particle Systems**: Custom particle effects
- **State Management**: Game state machine implementation

## 📦 Installation

### Prerequisites
- Visual Studio 2022
- SFML 2.6.2 library
- C++17 compatible compiler

### Steps
1. Install SFML 2.6.2 from [sfml-dev.org](https://www.sfml-dev.org/download/sfml/2.6.2/)
2. Configure Visual Studio:
   - Add SFML include directory to project properties
   - Add SFML lib directory to linker settings
   - Add these libraries to linker input: `sfml-graphics.lib;sfml-window.lib;sfml-system.lib;`
3. Copy SFML DLLs to the executable directory
4. Build and run the project

## 🎯 Controls

| Key | Action |
|-----|--------|
| **WASD** or **Arrow Keys** | Move player ship |
| **SPACE** | Shoot bullets |
| **P** | Pause/Resume game |
| **ENTER** | Start game / Return to menu |
| **ESC** | Exit game |

## 📁 Project Structure
SpaceShooter/
├── Main.cpp # Entry point and game loop
├── Classes/
│ ├── PlayerShip # Player character with movement and shooting
│ ├── EnemyShip # Enemy AI with different behaviors
│ ├── PowerUp # Collectible power-ups
│ └── ParticleSystem # Visual effects system
└── Game States/
├── Menu # Main menu interface
├── Playing # Active gameplay
├── Paused # Pause screen
└── GameOver # End game screen


## 🚀 How to Play

1. **Start the game** from the main menu
2. **Move your ship** to avoid enemies and collect power-ups
3. **Shoot enemies** to earn points and progress through waves
4. **Defeat the boss** every 3 waves for bonus points
5. **Survive as long as possible** to achieve high scores
6. **Collect power-ups** to heal and gain advantages

## 📊 Scoring System

| Action | Points |
|--------|--------|
| Destroy regular enemy | 100 |
| Destroy boss enemy | 500 |
| Collect power-up | 50 |

## 🔧 Technical Details

### Collision Detection
- Circle-based collision for bullets and enemies
- Rectangle-based collision for power-ups
- Efficient collision checking algorithms

### Enemy AI
- Random movement patterns
- Shooting based on timers
- Boss-specific behaviors and health bars

### Particle System
- GPU-accelerated particle rendering
- Customizable emitter properties
- Lifetime and color interpolation

### Performance Optimizations
- Object pooling for particles
- Efficient collision checking
- Smart enemy spawning

## 🎨 Visual Design

- **Player Ship**: Green rectangle with white outline
- **Enemies**: Red circles with varying sizes
- **Bullets**: Yellow circles with red outlines
- **Power-ups**: Colored squares with pulsing effects
- **Background**: Dark blue with moving stars

## 📈 Learning Outcomes

This project demonstrates:
- **SFML Graphics Programming**: Window management, shapes, text rendering
- **Game Physics**: Movement, collision detection, particle systems
- **AI Implementation**: Enemy behaviors and state machines
- **Game State Management**: Menu, gameplay, pause, game over states
- **OOP Principles**: Inheritance, polymorphism, encapsulation
- **Memory Management**: Smart pointers and efficient resource handling

## 🐛 Known Issues & Future Improvements

### Current Limitations
- Simple enemy AI patterns
- Basic particle effects
- Limited power-up variety

### Planned Features
- [ ] Multiple player ships
- [ ] Different weapon types
- [ ] Online high score system
- [ ] Sound effects and background music
- [ ] Level editor
- [ ] Mobile device support

## 🤝 Contributing

This is a learning project, but suggestions are welcome:
1. Fork the repository
2. Create a feature branch
3. Commit your changes
4. Push to the branch
5. Create a Pull Request

## 📄 License

This project is created for educational purposes as part of a 20-project C++ learning journey. Feel free to use the code for learning and experimentation.

## 👤 Author

**Melih Eneş Aktaş**
- 3D Artist & C++ Game Developer
- LinkedIn: [melihenesaktas](https://www.linkedin.com/in/melihenesaktas/)
- GitHub: [escapeesctr](https://github.com/escapeesctr)
- Portfolio: [https://escapeesctr.github.io/my-portfolio/](https://escapeesctr.github.io/my-portfolio/)

## 🌟 Acknowledgments

- SFML development team for the excellent graphics library
- Game development community for tutorials and resources
- All open-source projects that inspired this work

---

**Project Status**: Complete ✅ | **Difficulty Level**: Intermediate | **Estimated Development Time**: 8-10 hours

*Part of the 20-project C++ Game Development Portfolio Challenge*
