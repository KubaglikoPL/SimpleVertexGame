#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>

const static float PlayerSpeed = 200.0f;
const static float PlayerSize = 20.0f;
const static sf::Color PlayerColor = sf::Color::Cyan;

const static float BulletSize = 10.0f;
const static float PlayerBulletSpeed = 300.0f;
const static sf::Color PlayerBulletColor = sf::Color::Yellow;
const static sf::Color EnemyBulletColor = sf::Color::Red;

const static float PlayerBulletCooldown = 0.1f;

const static float EnemyBulletStartCooldown = 2.0f;

float random(float v_min, float v_max);
sf::Vector2f getMoveVector(float angle);
float angleBeetweenPoints(sf::Vector2f v, sf::Vector2f v2);
void spawnEnemyBullet();

class Player : public sf::Drawable, public sf::Transformable {
    sf::VertexArray vertex_array;
    float bullet_cooldown;
public:
    Player();
    void update(float delta_time);
    sf::FloatRect getAABB();
private:
    virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const {
        states.transform *= getTransform();

        target.draw(vertex_array, states);
    }
};

class Bullet : public sf::Drawable, public sf::Transformable {
    sf::VertexArray vertex_array;
    bool is_enemy;
public:
    Bullet(bool is_enemy);
    bool isEnemy();
    void update(float delta_time);
    sf::FloatRect getAABB();
    sf::Vector2f getVertex(int id);
private:
    virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const {
        states.transform *= getTransform();

        target.draw(vertex_array, states);
    }
};

class BulletManager : public sf::Drawable {
    std::vector<Bullet> bullets;
public:
    void addBullet(Bullet b);
    void update(float delta_time);
    void clearBullets();
private:
    virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const {
        for(size_t i = 0; i < bullets.size(); i++) {
            target.draw(bullets[i]);
        }
    }
};

class Button : public sf::Drawable, public sf::Transformable {
    sf::VertexArray vertex_array;
    sf::Text text;
public:
    Button(sf::Font *font, std::string text_string, sf::Color color, int width, int height);
    bool isPressed();
private:
    virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const {
        states.transform *= getTransform();

        target.draw(vertex_array, states);

        target.draw(text, states);
    }
};
