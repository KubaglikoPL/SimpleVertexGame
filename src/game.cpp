#include "game.hpp"
#include <math.h>
#include <iostream>
#include <random>
#include <time.h>
#include <sstream>

#define PI 3.14159265

float EnemyBulletCooldownDecressModifirer = 1.01f;
float EnemyBulletSpeed = 135.0f;

sf::RenderWindow window;
BulletManager bulletManager;
Player player;
float enemy_bullet_cooldown;
float enemy_bullet_decress;
float enemy_bullet_timer;
bool game_over_switch;
int score;

std::random_device dev;
std::mt19937 rng(dev());

std::string toString(int i) {
    std::ostringstream ss;
    ss << i;
    return ss.str();
}

float random(float v_min, float v_max) {
    std::uniform_real_distribution<> dist(v_min, v_max);
    return dist(rng);
}

float angleToRadians(float d) {
    return d * (PI/180.0f);
}

float radiansToAngle(float d) {
    return d * (180.0f/PI);
}

sf::Vector2f getMoveVector(float angle) {
    return sf::Vector2f(sin(angleToRadians(angle)), -cos(angleToRadians(angle)));
}

float angleBeetweenPoints(sf::Vector2f v, sf::Vector2f v2) {
    return radiansToAngle(atan2(v.y - v2.y, v.x - v2.x));
}

sf::Vector2f v2i_to_v2f(sf::Vector2i v) {
    return sf::Vector2f(v.x, v.y);
}

void spawnEnemyBullet() {
    Bullet b(true);

    float angle = random(0, 360);

    float x_pos = 800 * sin(angleToRadians(angle)) + 400;
    float y_pos = 800 * cos(angleToRadians(angle)) + 300;

    b.setPosition(x_pos, y_pos);

    float r = random(-15, 15);

    b.setRotation((-angle) + r);

    bulletManager.addBullet(b);
}

Player::Player() {
    vertex_array.clear();
    vertex_array.setPrimitiveType(sf::Lines);
    vertex_array.resize(8);
    vertex_array[0].position = sf::Vector2f(0, 0);
    vertex_array[1].position = sf::Vector2f(PlayerSize, 0);

    vertex_array[2].position = sf::Vector2f(PlayerSize, 0);
    vertex_array[3].position = sf::Vector2f(PlayerSize, PlayerSize);

    vertex_array[4].position = sf::Vector2f(PlayerSize, PlayerSize);
    vertex_array[5].position = sf::Vector2f(0, PlayerSize);

    vertex_array[6].position = sf::Vector2f(0, PlayerSize);
    vertex_array[7].position = sf::Vector2f(0, 0);

    for(size_t i = 0; i < vertex_array.getVertexCount(); i++) {
        vertex_array[i].color = PlayerColor;
    }

    setPosition(400 - PlayerSize/2.0f, 300 - PlayerSize/2.0f);
}

void Player::update(float delta_time) {
    sf::Vector2f prev_pos = getPosition();

    if(sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
        move(0, -PlayerSpeed * delta_time);
    }
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
        move(0, PlayerSpeed * delta_time);
    }
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
        move(-PlayerSpeed * delta_time, 0);
    }
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
        move(PlayerSpeed * delta_time, 0);
    }

    sf::FloatRect playerAABB = getAABB();
    sf::FloatRect gameBorderLeft = sf::FloatRect(-50, 0, 50, 600);
    sf::FloatRect gameBorderRight = sf::FloatRect(800, 0, 50, 600);
    sf::FloatRect gameBorderTop = sf::FloatRect(0, -50, 800, 50);
    sf::FloatRect gameBorderBottom = sf::FloatRect(0, 600, 800, 50);
    if((playerAABB.intersects(gameBorderLeft)) || (playerAABB.intersects(gameBorderRight)) || (playerAABB.intersects(gameBorderTop)) || (playerAABB.intersects(gameBorderBottom))) {
        setPosition(prev_pos);
    }

    bullet_cooldown += delta_time;

    if(bullet_cooldown >= PlayerBulletCooldown) {
        if(sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
            Bullet b(false);
            b.setOrigin(b.getAABB().width/2.0f, b.getAABB().height/2.0f);
            b.setRotation(angleBeetweenPoints(getPosition(), v2i_to_v2f(sf::Mouse::getPosition(window))) - 90.0f);
            b.setPosition(getPosition() + sf::Vector2f(PlayerSize/2.0f, PlayerSize/2.0f));
            bulletManager.addBullet(b);
            bullet_cooldown = 0;
        }
    }
}

sf::FloatRect Player::getAABB() {
    return getTransform().transformRect(vertex_array.getBounds());
}

Bullet::Bullet(bool is_enemy) {
    vertex_array.clear();
    vertex_array.setPrimitiveType(sf::Lines);
    vertex_array.resize(6);

    vertex_array[0].position = sf::Vector2f(0, BulletSize);
    vertex_array[1].position = sf::Vector2f(BulletSize, BulletSize);

    vertex_array[2].position = sf::Vector2f(BulletSize, BulletSize);
    vertex_array[3].position = sf::Vector2f(BulletSize/2.0f, 0);

    vertex_array[4].position = sf::Vector2f(BulletSize/2.0f, 0);
    vertex_array[5].position = sf::Vector2f(0, BulletSize);

    this->is_enemy = is_enemy;

    for(size_t i = 0; i < vertex_array.getVertexCount(); i++) {
        if(is_enemy) vertex_array[i].color = EnemyBulletColor;
        else vertex_array[i].color = PlayerBulletColor;
    }
}

bool Bullet::isEnemy() {
    return is_enemy;
}

void Bullet::update(float delta_time) {
    if(!is_enemy) move(getMoveVector(getRotation()) * PlayerBulletSpeed * delta_time);
    if(is_enemy) move(getMoveVector(getRotation()) * EnemyBulletSpeed * delta_time);
}

sf::FloatRect Bullet::getAABB() {
    return getTransform().transformRect(vertex_array.getBounds());
}

void BulletManager::addBullet(Bullet b) {
    bullets.push_back(b);
}

void BulletManager::update(float delta_time) {
    sf::FloatRect r = sf::FloatRect(-1000, -1000, 3200, 3000);

    for(size_t i = 0; i < bullets.size(); i++) {
        if(!bullets[i].getAABB().intersects(r)) {
            bullets.erase(bullets.begin() + i);
            break;
        }
    }

    //std::cout << bullets.size() << std::endl;
    for(size_t i = 0; i < bullets.size(); i++) {
        bullets[i].update(delta_time);
    }

    for(size_t i = 0; i < bullets.size(); i++) {
        if(bullets[i].isEnemy()) {
            if(bullets[i].getAABB().intersects(player.getAABB())) {
                game_over_switch = true;
            }
        }
    }

    for(size_t i = 0; i < bullets.size(); i++) {
        for(size_t i2 = 0; i2 < bullets.size(); i2++) {
            if(i != i2) {
                if((bullets[i].isEnemy()) && (!bullets[i2].isEnemy())) {
                    if(bullets[i].getAABB().intersects(bullets[i2].getAABB())) {
                        bullets.erase(bullets.begin() + i);
                        bullets.erase(bullets.begin() + i2);
                        score++;
                        break;
                        break;
                    }
                }
            }
        }
    }
}

void BulletManager::clearBullets() {
    bullets.clear();
}

int main() {
    srand(time(0));

    sf::ContextSettings ctx_settings;
    ctx_settings.antialiasingLevel = 2;
    window.create(sf::VideoMode(800, 600), "Game", sf::Style::Default, ctx_settings);
    window.setFramerateLimit(60);
    window.setVerticalSyncEnabled(true);

    enemy_bullet_cooldown = EnemyBulletStartCooldown;
    enemy_bullet_decress = EnemyBulletCooldownDecressModifirer;

    sf::Font font;
    font.loadFromFile("data/Tuffy_Bold.ttf");
    sf::Text score_text("", font);
    sf::Text game_over("GAME OVER", font, 72);
    game_over.setOrigin(game_over.getGlobalBounds().width/2, game_over.getGlobalBounds().height/2);
    game_over.setPosition(400, 300);
    sf::Text game_over_bottom("PRESS R TO RESTART", font);
    game_over_bottom.setOrigin(game_over.getGlobalBounds().width/2, game_over.getGlobalBounds().height/2);
    game_over_bottom.setPosition(400, 380);

    game_over_switch = false;
    score = 0;

    sf::Event e;
    sf::Clock game_timer;
    float delta_time;
    float dificulty_up_timer = 0;
    while(window.isOpen()) {
        while(window.pollEvent(e)) {
            if(e.type == sf::Event::Closed) window.close();
        }
        delta_time = game_timer.restart().asSeconds();
        if(!game_over_switch) {
            player.update(delta_time);
            bulletManager.update(delta_time);
        }
        score_text.setString("Score: " + toString(score));

        enemy_bullet_timer += delta_time;
        if(!game_over_switch) {
            if(enemy_bullet_timer >= enemy_bullet_cooldown) {
                spawnEnemyBullet();
                enemy_bullet_timer = 0;
                score++;
            }
        }

        dificulty_up_timer += delta_time;
        if(dificulty_up_timer > DifficultyUpTime) {
            enemy_bullet_decress = enemy_bullet_decress * EnemyBulletCooldownDecressModifirer;
            enemy_bullet_cooldown = EnemyBulletStartCooldown - enemy_bullet_decress;
            dificulty_up_timer = 0;
        }

        window.clear();
        window.draw(player);
        window.draw(bulletManager);
        if(game_over_switch) {
            window.draw(game_over);
            window.draw(game_over_bottom);
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::R)) {
                player.setPosition(400 - PlayerSize/2.0f, 300 - PlayerSize/2.0f);
                bulletManager.clearBullets();
                score = 0;
                enemy_bullet_cooldown = EnemyBulletStartCooldown;
                enemy_bullet_decress = EnemyBulletCooldownDecressModifirer;
                game_over_switch = false;
            }
        }
        window.draw(score_text);
        window.display();
    }

    return 0;
}
