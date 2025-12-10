#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <sstream>
#include <memory>

using namespace sf;
using namespace std;

const int WINDOW_WIDTH = 1200;
const int WINDOW_HEIGHT = 800;
const float PI = 3.14159265f;

enum GameState {
    MENU,
    PLAYING,
    GAME_OVER,
    PAUSED
};

struct Velocity {
    float x, y;
    Velocity(float x = 0, float y = 0) : x(x), y(y) {}
};

class ParticleSystem : public Drawable {
private:
    struct Particle {
        Vector2f position;
        Vector2f velocity;
        Color color;
        float lifetime;
        float maxLifetime;
        float size;

        Particle() : position(0, 0), velocity(0, 0), color(Color::White),
            lifetime(0), maxLifetime(0), size(0) {
        }
    };

    vector<Particle> particles;
    VertexArray vertices;
    bool emitting;
    Vector2f emitterPosition;

public:
    ParticleSystem() : vertices(Points), emitting(false), emitterPosition(0, 0) {
        vertices.resize(1000);
    }

    void setEmitter(Vector2f position) {
        emitterPosition = position;
    }

    void startEmission() {
        emitting = true;
    }

    void stopEmission() {
        emitting = false;
    }

    void update(float deltaTime) {
        if (emitting && particles.size() < 500) {
            for (int i = 0; i < 5; ++i) {
                Particle p;
                p.position = emitterPosition;
                float angle = (rand() % 360) * PI / 180.f;
                float speed = (rand() % 50) + 50.f;
                p.velocity = Vector2f(cos(angle) * speed, sin(angle) * speed);
                p.color = Color(rand() % 255, rand() % 255, rand() % 255);
                p.lifetime = p.maxLifetime = (rand() % 100) / 100.f + 0.5f;
                p.size = (rand() % 10) + 5.f;
                particles.push_back(p);
            }
        }

        for (size_t i = 0; i < particles.size();) {
            particles[i].lifetime -= deltaTime;
            particles[i].position += particles[i].velocity * deltaTime;
            particles[i].velocity.y += 100.f * deltaTime;

            if (particles[i].lifetime <= 0) {
                particles.erase(particles.begin() + i);
            }
            else {
                ++i;
            }
        }

        vertices.clear();
        vertices.resize(particles.size());
        for (size_t i = 0; i < particles.size(); ++i) {
            float alpha = particles[i].lifetime / particles[i].maxLifetime;
            Color particleColor = particles[i].color;
            particleColor.a = static_cast<Uint8>(alpha * 255);
            vertices[i].position = particles[i].position;
            vertices[i].color = particleColor;
        }
    }

    void draw(RenderTarget& target, RenderStates states) const override {
        target.draw(vertices, states);
    }
};

class PlayerShip {
private:
    RectangleShape shape;
    Vector2f position;
    Velocity velocity;
    float speed;
    int health;
    bool isAlive;
    float shootCooldown;
    float maxShootCooldown;
    int score;
    Clock invincibilityClock;
    bool isInvincible;

public:
    PlayerShip() : position(WINDOW_WIDTH / 2, WINDOW_HEIGHT - 100),
        velocity(0, 0), speed(500.f), health(100), isAlive(true),
        shootCooldown(0), maxShootCooldown(0.2f), score(0),
        isInvincible(false) {
        shape.setSize(Vector2f(60, 40));
        shape.setFillColor(Color::Green);
        shape.setOutlineThickness(2);
        shape.setOutlineColor(Color::White);
        shape.setOrigin(30, 20);
        shape.setPosition(position);
    }

    void update(float deltaTime) {
        if (!isAlive) return;

        velocity.x = 0;
        velocity.y = 0;

        if (Keyboard::isKeyPressed(Keyboard::A) || Keyboard::isKeyPressed(Keyboard::Left)) {
            velocity.x = -speed;
        }
        if (Keyboard::isKeyPressed(Keyboard::D) || Keyboard::isKeyPressed(Keyboard::Right)) {
            velocity.x = speed;
        }
        if (Keyboard::isKeyPressed(Keyboard::W) || Keyboard::isKeyPressed(Keyboard::Up)) {
            velocity.y = -speed;
        }
        if (Keyboard::isKeyPressed(Keyboard::S) || Keyboard::isKeyPressed(Keyboard::Down)) {
            velocity.y = speed;
        }

        position.x += velocity.x * deltaTime;
        position.y += velocity.y * deltaTime;

        float halfWidth = shape.getSize().x / 2;
        float halfHeight = shape.getSize().y / 2;

        if (position.x < halfWidth) position.x = halfWidth;
        if (position.x > WINDOW_WIDTH - halfWidth) position.x = WINDOW_WIDTH - halfWidth;
        if (position.y < halfHeight) position.y = halfHeight;
        if (position.y > WINDOW_HEIGHT - halfHeight) position.y = WINDOW_HEIGHT - halfHeight;

        shape.setPosition(position);

        if (shootCooldown > 0) {
            shootCooldown -= deltaTime;
        }

        if (isInvincible && invincibilityClock.getElapsedTime().asSeconds() > 1.5f) {
            isInvincible = false;
            shape.setFillColor(Color::Green);
        }
    }

    bool canShoot() const {
        return shootCooldown <= 0 && isAlive;
    }

    void shoot() {
        shootCooldown = maxShootCooldown;
    }

    CircleShape createBullet() const {
        CircleShape bullet(5);
        bullet.setFillColor(Color::Yellow);
        bullet.setOutlineColor(Color::Red);
        bullet.setOutlineThickness(2);
        bullet.setOrigin(5, 5);
        bullet.setPosition(position.x, position.y - 30);
        return bullet;
    }

    void takeDamage(int damage) {
        if (isInvincible) return;

        health -= damage;
        if (health <= 0) {
            health = 0;
            isAlive = false;
            shape.setFillColor(Color::Transparent);
        }
        else {
            isInvincible = true;
            invincibilityClock.restart();
            shape.setFillColor(Color(255, 100, 100, 150));
        }
    }

    void addScore(int points) {
        score += points;
    }

    void heal(int amount) {
        health += amount;
        if (health > 100) health = 100;
    }

    const RectangleShape& getShape() const { return shape; }
    const Vector2f& getPosition() const { return position; }
    int getHealth() const { return health; }
    bool getIsAlive() const { return isAlive; }
    int getScore() const { return score; }
    float getShootCooldown() const { return shootCooldown; }
    float getMaxShootCooldown() const { return maxShootCooldown; }
};

class EnemyShip {
private:
    CircleShape shape;
    Vector2f position;
    Velocity velocity;
    int health;
    int maxHealth;
    int damage;
    int points;
    float shootTimer;
    float shootInterval;
    bool isBoss;

public:
    EnemyShip(bool boss = false) : isBoss(boss), position(0, 0), velocity(0, 0),
        health(0), maxHealth(0), damage(0), points(0),
        shootTimer(0), shootInterval(0) {
        if (boss) {
            shape.setRadius(40);
            shape.setFillColor(Color(200, 50, 50));
            shape.setOutlineColor(Color::Red);
            shape.setOutlineThickness(2);
            maxHealth = health = 500;
            damage = 30;
            points = 500;
            shootInterval = 1.5f;
            velocity = Velocity(0, 50);
        }
        else {
            shape.setRadius(20);
            shape.setFillColor(Color::Red);
            shape.setOutlineColor(Color(255, 100, 100));
            shape.setOutlineThickness(2);
            maxHealth = health = 50;
            damage = 10;
            points = 100;
            shootInterval = 2.0f;
            velocity = Velocity((rand() % 100) - 50, (rand() % 50) + 50);
        }

        shape.setOrigin(shape.getRadius(), shape.getRadius());

        position = Vector2f(rand() % (WINDOW_WIDTH - 100) + 50, -50);
        shape.setPosition(position);

        shootTimer = (rand() % 100) / 100.f * shootInterval;
    }

    void update(float deltaTime) {
        position.x += velocity.x * deltaTime;
        position.y += velocity.y * deltaTime;

        float radius = shape.getRadius();
        if (position.x < radius) {
            position.x = radius;
            velocity.x = -velocity.x;
        }
        if (position.x > WINDOW_WIDTH - radius) {
            position.x = WINDOW_WIDTH - radius;
            velocity.x = -velocity.x;
        }

        shape.setPosition(position);

        shootTimer += deltaTime;
    }

    bool canShoot() const {
        return shootTimer >= shootInterval;
    }

    void resetShootTimer() {
        shootTimer = 0;
    }

    CircleShape createBullet() const {
        CircleShape bullet(4);
        bullet.setFillColor(Color::Magenta);
        bullet.setOutlineColor(Color(255, 100, 255));
        bullet.setOutlineThickness(1);
        bullet.setOrigin(4, 4);
        bullet.setPosition(position.x, position.y + shape.getRadius() + 10);
        return bullet;
    }

    void takeDamage(int damage) {
        health -= damage;
        float healthPercent = static_cast<float>(health) / maxHealth;

        if (isBoss) {
            Color newColor(200, static_cast<Uint8>(50 * healthPercent),
                static_cast<Uint8>(50 * healthPercent));
            shape.setFillColor(newColor);
        }
    }

    bool isAlive() const {
        return health > 0;
    }

    bool isOffScreen() const {
        return position.y > WINDOW_HEIGHT + 100;
    }

    const CircleShape& getShape() const { return shape; }
    const Vector2f& getPosition() const { return position; }
    int getDamage() const { return damage; }
    int getPoints() const { return points; }
    int getHealth() const { return health; }
    int getMaxHealth() const { return maxHealth; }
    bool getIsBoss() const { return isBoss; }
    float getRadius() const { return shape.getRadius(); }
};

class PowerUp {
private:
    RectangleShape shape;
    Vector2f position;
    Velocity velocity;
    int type;
    float activeTime;

public:
    PowerUp(Vector2f pos) : position(pos), velocity(0, 100), type(0), activeTime(10.0f) {
        type = rand() % 3;

        shape.setSize(Vector2f(30, 30));
        shape.setOrigin(15, 15);
        shape.setPosition(position);

        switch (type) {
        case 0:
            shape.setFillColor(Color::Green);
            break;
        case 1:
            shape.setFillColor(Color::Cyan);
            break;
        case 2:
            shape.setFillColor(Color::Yellow);
            break;
        }

        shape.setOutlineColor(Color::White);
        shape.setOutlineThickness(2);
    }

    void update(float deltaTime) {
        position += Vector2f(velocity.x * deltaTime, velocity.y * deltaTime);
        shape.setPosition(position);

        float pulse = sin(activeTime * 5) * 0.5f + 0.5f;
        shape.setFillColor(Color(
            static_cast<Uint8>(shape.getFillColor().r * pulse),
            static_cast<Uint8>(shape.getFillColor().g * pulse),
            static_cast<Uint8>(shape.getFillColor().b * pulse)
        ));

        activeTime += deltaTime;
    }

    bool isOffScreen() const {
        return position.y > WINDOW_HEIGHT + 50;
    }

    void applyEffect(PlayerShip& player) {
        switch (type) {
        case 0:
            player.heal(30);
            break;
        }
    }

    const RectangleShape& getShape() const { return shape; }
    const Vector2f& getPosition() const { return position; }
    int getType() const { return type; }
};

class SpaceShooterGame {
private:
    RenderWindow window;
    GameState currentState;

    PlayerShip player;

    vector<EnemyShip> enemies;
    float enemySpawnTimer;
    float enemySpawnInterval;
    int waveNumber;
    int enemiesPerWave;
    int enemiesSpawnedThisWave;
    bool bossSpawned;

    vector<CircleShape> playerBullets;
    vector<CircleShape> enemyBullets;

    vector<PowerUp> powerUps;
    float powerUpSpawnTimer;

    vector<unique_ptr<ParticleSystem>> particleSystems;

    Clock gameClock;
    float deltaTime;

    RectangleShape background;
    vector<RectangleShape> stars;

    Font font;
    bool fontLoaded;

public:
    SpaceShooterGame() : window(VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Space Shooter - Proje 13"),
        currentState(MENU), enemySpawnTimer(0), enemySpawnInterval(1.0f),
        waveNumber(1), enemiesPerWave(5), enemiesSpawnedThisWave(0),
        bossSpawned(false), powerUpSpawnTimer(10.0f), deltaTime(0),
        fontLoaded(false) {
        window.setFramerateLimit(60);
        srand(static_cast<unsigned>(time(nullptr)));

        setupBackground();
        setupFont();
    }

private:
    void setupBackground() {
        background.setSize(Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT));
        background.setFillColor(Color(10, 10, 40));

        for (int i = 0; i < 100; ++i) {
            RectangleShape star(Vector2f(2, 2));
            star.setFillColor(Color::White);
            star.setPosition(rand() % WINDOW_WIDTH, rand() % WINDOW_HEIGHT);
            stars.push_back(star);
        }
    }

    void setupFont() {
        fontLoaded = font.loadFromFile("C:\\Windows\\Fonts\\arial.ttf");
        if (!fontLoaded) {
            cout << "Font yuklenemedi! Textler gorunmeyebilir." << endl;
        }
    }

    void spawnEnemy() {
        if (enemiesSpawnedThisWave < enemiesPerWave) {
            enemies.push_back(EnemyShip(false));
            enemiesSpawnedThisWave++;
        }
        else if (!bossSpawned && waveNumber % 3 == 0) {
            enemies.push_back(EnemyShip(true));
            bossSpawned = true;
        }
    }

    void spawnPowerUp(Vector2f position) {
        if (rand() % 100 < 10) {
            powerUps.push_back(PowerUp(position));
        }
    }

    void checkCollisions() {
        for (size_t i = 0; i < playerBullets.size(); ++i) {
            for (size_t j = 0; j < enemies.size(); ++j) {
                if (enemies[j].isAlive()) {
                    Vector2f bulletPos = playerBullets[i].getPosition();
                    Vector2f enemyPos = enemies[j].getPosition();
                    float enemyRadius = enemies[j].getRadius();

                    float dx = bulletPos.x - enemyPos.x;
                    float dy = bulletPos.y - enemyPos.y;
                    float distance = sqrt(dx * dx + dy * dy);

                    if (distance < enemyRadius + 5) {
                        enemies[j].takeDamage(25);

                        auto ps = make_unique<ParticleSystem>();
                        ps->setEmitter(bulletPos);
                        ps->startEmission();
                        particleSystems.push_back(move(ps));

                        if (!enemies[j].isAlive()) {
                            player.addScore(enemies[j].getPoints());
                            spawnPowerUp(enemies[j].getPosition());

                            for (int k = 0; k < 3; ++k) {
                                auto ps2 = make_unique<ParticleSystem>();
                                ps2->setEmitter(enemies[j].getPosition());
                                ps2->startEmission();
                                particleSystems.push_back(move(ps2));
                            }
                        }

                        playerBullets.erase(playerBullets.begin() + i);
                        i--;
                        break;
                    }
                }
            }
        }

        for (size_t i = 0; i < enemyBullets.size(); ++i) {
            FloatRect playerBounds = player.getShape().getGlobalBounds();
            FloatRect bulletBounds = enemyBullets[i].getGlobalBounds();

            if (playerBounds.intersects(bulletBounds)) {
                player.takeDamage(10);
                enemyBullets.erase(enemyBullets.begin() + i);
                i--;

                auto ps = make_unique<ParticleSystem>();
                ps->setEmitter(player.getPosition());
                ps->startEmission();
                particleSystems.push_back(move(ps));
            }
        }

        if (player.getIsAlive()) {
            FloatRect playerBounds = player.getShape().getGlobalBounds();

            for (size_t i = 0; i < enemies.size(); ++i) {
                if (enemies[i].isAlive()) {
                    CircleShape enemyShape = enemies[i].getShape();
                    FloatRect enemyBounds = enemyShape.getGlobalBounds();

                    if (playerBounds.intersects(enemyBounds)) {
                        player.takeDamage(enemies[i].getDamage());
                        enemies[i].takeDamage(100);

                        for (int j = 0; j < 5; ++j) {
                            auto ps = make_unique<ParticleSystem>();
                            ps->setEmitter(enemies[i].getPosition());
                            ps->startEmission();
                            particleSystems.push_back(move(ps));
                        }
                    }
                }
            }
        }

        for (size_t i = 0; i < powerUps.size(); ++i) {
            FloatRect playerBounds = player.getShape().getGlobalBounds();
            FloatRect powerUpBounds = powerUps[i].getShape().getGlobalBounds();

            if (playerBounds.intersects(powerUpBounds)) {
                powerUps[i].applyEffect(player);
                powerUps.erase(powerUps.begin() + i);
                i--;

                auto ps = make_unique<ParticleSystem>();
                ps->setEmitter(player.getPosition());
                ps->startEmission();
                particleSystems.push_back(move(ps));
            }
        }
    }

    void nextWave() {
        waveNumber++;
        enemiesPerWave += 2;
        enemiesSpawnedThisWave = 0;
        bossSpawned = false;
        enemySpawnInterval = max(0.3f, 1.0f - waveNumber * 0.05f);
    }

    void handleInput() {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) {
                window.close();
            }

            if (event.type == Event::KeyPressed) {
                switch (currentState) {
                case MENU:
                    if (event.key.code == Keyboard::Enter) {
                        currentState = PLAYING;
                        resetGame();
                    }
                    else if (event.key.code == Keyboard::Escape) {
                        window.close();
                    }
                    break;

                case PLAYING:
                    if (event.key.code == Keyboard::P) {
                        currentState = PAUSED;
                    }
                    else if (event.key.code == Keyboard::Space) {
                        if (player.canShoot()) {
                            playerBullets.push_back(player.createBullet());
                            player.shoot();
                        }
                    }
                    break;

                case GAME_OVER:
                    if (event.key.code == Keyboard::Enter) {
                        currentState = MENU;
                    }
                    break;

                case PAUSED:
                    if (event.key.code == Keyboard::P) {
                        currentState = PLAYING;
                    }
                    break;
                }
            }
        }

        if (currentState == PLAYING && player.getIsAlive()) {
            if (Keyboard::isKeyPressed(Keyboard::Space) && player.canShoot()) {
                playerBullets.push_back(player.createBullet());
                player.shoot();
            }
        }
    }

    void resetGame() {
        player = PlayerShip();
        enemies.clear();
        playerBullets.clear();
        enemyBullets.clear();
        powerUps.clear();
        particleSystems.clear();

        waveNumber = 1;
        enemiesPerWave = 5;
        enemiesSpawnedThisWave = 0;
        bossSpawned = false;
        enemySpawnInterval = 1.0f;
        enemySpawnTimer = 0;
        powerUpSpawnTimer = 10.0f;
    }

    void update(float dt) {
        deltaTime = dt;

        switch (currentState) {
        case PLAYING:
            updateGameplay(dt);
            break;
        case GAME_OVER:
            updateGameOver(dt);
            break;
        case PAUSED:
            updatePaused(dt);
            break;
        case MENU:
            updateMenu(dt);
            break;
        }
    }

    void updateGameplay(float dt) {
        player.update(dt);

        enemySpawnTimer += dt;
        if (enemySpawnTimer >= enemySpawnInterval) {
            spawnEnemy();
            enemySpawnTimer = 0;
        }

        powerUpSpawnTimer += dt;
        if (powerUpSpawnTimer >= 15.0f) {
            Vector2f spawnPos(rand() % (WINDOW_WIDTH - 100) + 50, -50);
            powerUps.push_back(PowerUp(spawnPos));
            powerUpSpawnTimer = 0;
        }

        for (size_t i = 0; i < enemies.size();) {
            enemies[i].update(dt);

            if (enemies[i].canShoot() && enemies[i].isAlive()) {
                enemyBullets.push_back(enemies[i].createBullet());
                enemies[i].resetShootTimer();
            }

            if (enemies[i].isOffScreen() || !enemies[i].isAlive()) {
                enemies.erase(enemies.begin() + i);
            }
            else {
                ++i;
            }
        }

        if (enemiesSpawnedThisWave >= enemiesPerWave && enemies.empty()) {
            nextWave();
        }

        for (size_t i = 0; i < playerBullets.size();) {
            playerBullets[i].move(0, -800 * dt);
            if (playerBullets[i].getPosition().y < -10) {
                playerBullets.erase(playerBullets.begin() + i);
            }
            else {
                ++i;
            }
        }

        for (size_t i = 0; i < enemyBullets.size();) {
            enemyBullets[i].move(0, 400 * dt);
            if (enemyBullets[i].getPosition().y > WINDOW_HEIGHT + 10) {
                enemyBullets.erase(enemyBullets.begin() + i);
            }
            else {
                ++i;
            }
        }

        for (size_t i = 0; i < powerUps.size();) {
            powerUps[i].update(dt);
            if (powerUps[i].isOffScreen()) {
                powerUps.erase(powerUps.begin() + i);
            }
            else {
                ++i;
            }
        }

        for (size_t i = 0; i < particleSystems.size();) {
            particleSystems[i]->update(dt);

            if (i > 10) {
                particleSystems.erase(particleSystems.begin());
            }
            else {
                ++i;
            }
        }

        checkCollisions();

        if (!player.getIsAlive()) {
            currentState = GAME_OVER;
        }
    }

    void updateGameOver(float dt) {
        for (auto& ps : particleSystems) {
            ps->update(dt);
        }
    }

    void updatePaused(float dt) {
    }

    void updateMenu(float dt) {
        for (auto& star : stars) {
            star.move(0, 50 * dt);
            if (star.getPosition().y > WINDOW_HEIGHT) {
                star.setPosition(rand() % WINDOW_WIDTH, -10);
            }
        }
    }

    void render() {
        window.clear();

        window.draw(background);

        for (const auto& star : stars) {
            window.draw(star);
        }

        switch (currentState) {
        case MENU:
            renderMenu();
            break;

        case PLAYING:
            renderGame();
            renderUI();
            break;

        case GAME_OVER:
            renderGame();
            renderGameOver();
            break;

        case PAUSED:
            renderGame();
            renderUI();
            renderPaused();
            break;
        }

        window.display();
    }

    void renderMenu() {
        RectangleShape titleBox(Vector2f(600, 300));
        titleBox.setFillColor(Color(0, 0, 0, 200));
        titleBox.setOutlineColor(Color::Yellow);
        titleBox.setOutlineThickness(3);
        titleBox.setOrigin(300, 150);
        titleBox.setPosition(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
        window.draw(titleBox);

        if (fontLoaded) {
            Text titleText;
            titleText.setFont(font);
            titleText.setString("SPACE SHOOTER");
            titleText.setCharacterSize(60);
            titleText.setFillColor(Color::Yellow);
            titleText.setStyle(Text::Bold);
            FloatRect titleBounds = titleText.getLocalBounds();
            titleText.setOrigin(titleBounds.width / 2, titleBounds.height / 2);
            titleText.setPosition(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 - 80);
            window.draw(titleText);

            Text startText;
            startText.setFont(font);
            startText.setString("Press ENTER to Start");
            startText.setCharacterSize(32);
            startText.setFillColor(Color::Green);
            FloatRect startBounds = startText.getLocalBounds();
            startText.setOrigin(startBounds.width / 2, startBounds.height / 2);
            startText.setPosition(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
            window.draw(startText);

            Text exitText;
            exitText.setFont(font);
            exitText.setString("Press ESC to Exit");
            exitText.setCharacterSize(32);
            exitText.setFillColor(Color::Red);
            FloatRect exitBounds = exitText.getLocalBounds();
            exitText.setOrigin(exitBounds.width / 2, exitBounds.height / 2);
            exitText.setPosition(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 + 50);
            window.draw(exitText);
        }
        else {
            RectangleShape startButton(Vector2f(300, 50));
            startButton.setFillColor(Color::Green);
            startButton.setOutlineColor(Color::White);
            startButton.setOutlineThickness(2);
            startButton.setOrigin(150, 25);
            startButton.setPosition(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
            window.draw(startButton);

            RectangleShape exitButton(Vector2f(300, 50));
            exitButton.setFillColor(Color::Red);
            exitButton.setOutlineColor(Color::White);
            exitButton.setOutlineThickness(2);
            exitButton.setOrigin(150, 25);
            exitButton.setPosition(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 + 70);
            window.draw(exitButton);
        }

        if (fontLoaded) {
            Text controlsText;
            controlsText.setFont(font);
            controlsText.setString("Controls: WASD/Arrows = Move, SPACE = Shoot, P = Pause");
            controlsText.setCharacterSize(20);
            controlsText.setFillColor(Color::White);
            controlsText.setPosition(20, WINDOW_HEIGHT - 40);
            window.draw(controlsText);
        }
    }

    void renderGame() {
        if (player.getIsAlive()) {
            window.draw(player.getShape());
        }

        for (const auto& enemy : enemies) {
            if (enemy.isAlive()) {
                window.draw(enemy.getShape());

                if (enemy.getIsBoss()) {
                    RectangleShape healthBar(Vector2f(100, 10));
                    healthBar.setFillColor(Color::Red);
                    healthBar.setOutlineColor(Color::White);
                    healthBar.setOutlineThickness(1);
                    healthBar.setPosition(enemy.getPosition().x - 50,
                        enemy.getPosition().y - 60);

                    float healthPercent = static_cast<float>(enemy.getHealth()) / enemy.getMaxHealth();
                    RectangleShape healthFill(Vector2f(100 * healthPercent, 10));
                    healthFill.setFillColor(Color::Green);
                    healthFill.setPosition(enemy.getPosition().x - 50,
                        enemy.getPosition().y - 60);

                    window.draw(healthBar);
                    window.draw(healthFill);
                }
            }
        }

        for (const auto& bullet : playerBullets) {
            window.draw(bullet);
        }
        for (const auto& bullet : enemyBullets) {
            window.draw(bullet);
        }

        for (const auto& powerUp : powerUps) {
            window.draw(powerUp.getShape());
        }

        for (const auto& ps : particleSystems) {
            window.draw(*ps);
        }

        if (player.getIsAlive()) {
            RectangleShape playerHealthBar(Vector2f(200, 15));
            playerHealthBar.setFillColor(Color::Black);
            playerHealthBar.setOutlineColor(Color::White);
            playerHealthBar.setOutlineThickness(1);
            playerHealthBar.setPosition(WINDOW_WIDTH - 220, 20);

            float playerHealthPercent = player.getHealth() / 100.f;
            RectangleShape playerHealthFill(Vector2f(200 * playerHealthPercent, 15));
            playerHealthFill.setFillColor(Color::Green);
            playerHealthFill.setPosition(WINDOW_WIDTH - 220, 20);

            window.draw(playerHealthBar);
            window.draw(playerHealthFill);
        }

        RectangleShape cooldownBar(Vector2f(200, 10));
        cooldownBar.setFillColor(Color::Black);
        cooldownBar.setOutlineColor(Color::White);
        cooldownBar.setOutlineThickness(1);
        cooldownBar.setPosition(WINDOW_WIDTH - 220, 45);

        float cooldownPercent = player.getShootCooldown() / player.getMaxShootCooldown();
        RectangleShape cooldownFill(Vector2f(200 * (1 - cooldownPercent), 10));
        cooldownFill.setFillColor(Color::Yellow);
        cooldownFill.setPosition(WINDOW_WIDTH - 220, 45);

        window.draw(cooldownBar);
        window.draw(cooldownFill);
    }

    void renderUI() {
        if (!fontLoaded) return;

        Text scoreText;
        scoreText.setFont(font);
        scoreText.setString("Score: " + to_string(player.getScore()));
        scoreText.setCharacterSize(24);
        scoreText.setFillColor(Color::White);
        scoreText.setPosition(20, 20);
        window.draw(scoreText);

        Text healthText;
        healthText.setFont(font);
        healthText.setString("Health: " + to_string(player.getHealth()));
        healthText.setCharacterSize(24);
        healthText.setFillColor(Color::Green);
        healthText.setPosition(20, 50);
        window.draw(healthText);

        Text waveText;
        waveText.setFont(font);
        waveText.setString("Wave: " + to_string(waveNumber));
        waveText.setCharacterSize(24);
        waveText.setFillColor(Color::Cyan);
        waveText.setPosition(20, 80);
        window.draw(waveText);

        Text controlsText;
        controlsText.setFont(font);
        controlsText.setString("Controls: WASD/Arrows = Move, SPACE = Shoot, P = Pause");
        controlsText.setCharacterSize(18);
        controlsText.setFillColor(Color(200, 200, 200));
        controlsText.setPosition(20, WINDOW_HEIGHT - 40);
        window.draw(controlsText);
    }

    void renderGameOver() {
        RectangleShape overlay(Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT));
        overlay.setFillColor(Color(0, 0, 0, 150));
        window.draw(overlay);

        if (!fontLoaded) return;

        Text gameOverText;
        gameOverText.setFont(font);
        gameOverText.setString("GAME OVER");
        gameOverText.setCharacterSize(72);
        gameOverText.setFillColor(Color::Red);
        gameOverText.setStyle(Text::Bold);
        FloatRect gameOverBounds = gameOverText.getLocalBounds();
        gameOverText.setOrigin(gameOverBounds.width / 2, gameOverBounds.height / 2);
        gameOverText.setPosition(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 - 50);
        window.draw(gameOverText);

        Text finalScore;
        finalScore.setFont(font);
        finalScore.setString("Final Score: " + to_string(player.getScore()));
        finalScore.setCharacterSize(36);
        finalScore.setFillColor(Color::White);
        FloatRect scoreBounds = finalScore.getLocalBounds();
        finalScore.setOrigin(scoreBounds.width / 2, scoreBounds.height / 2);
        finalScore.setPosition(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 + 30);
        window.draw(finalScore);

        Text restartText;
        restartText.setFont(font);
        restartText.setString("Press ENTER to return to Menu");
        restartText.setCharacterSize(24);
        restartText.setFillColor(Color::Yellow);
        FloatRect restartBounds = restartText.getLocalBounds();
        restartText.setOrigin(restartBounds.width / 2, restartBounds.height / 2);
        restartText.setPosition(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 + 100);
        window.draw(restartText);
    }

    void renderPaused() {
        RectangleShape overlay(Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT));
        overlay.setFillColor(Color(0, 0, 0, 150));
        window.draw(overlay);

        if (!fontLoaded) return;

        Text pauseText;
        pauseText.setFont(font);
        pauseText.setString("PAUSED");
        pauseText.setCharacterSize(72);
        pauseText.setFillColor(Color::White);
        pauseText.setStyle(Text::Bold);
        FloatRect pauseBounds = pauseText.getLocalBounds();
        pauseText.setOrigin(pauseBounds.width / 2, pauseBounds.height / 2);
        pauseText.setPosition(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 - 50);
        window.draw(pauseText);

        Text resumeText;
        resumeText.setFont(font);
        resumeText.setString("Press P to Resume");
        resumeText.setCharacterSize(36);
        resumeText.setFillColor(Color::Yellow);
        FloatRect resumeBounds = resumeText.getLocalBounds();
        resumeText.setOrigin(resumeBounds.width / 2, resumeBounds.height / 2);
        resumeText.setPosition(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 + 50);
        window.draw(resumeText);
    }

public:
    void run() {
        cout << "Space Shooter baslatiliyor..." << endl;
        cout << "Kontroller: WASD/Ok Tuslari = Hareket, SPACE = Ates Et, P = Duraklat" << endl;

        while (window.isOpen()) {
            float dt = gameClock.restart().asSeconds();

            handleInput();
            update(dt);
            render();
        }
    }
};

int main() {
    SpaceShooterGame game;
    game.run();
    return 0;

}
