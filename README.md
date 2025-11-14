# QMaterialWidget

Виджет Qt с эффектами Material Design: elevation (поднятие), динамические тени и ripple-эффект.

## Описание

`QMaterialWidget` — это кастомный виджет Qt, реализующий основные принципы Material Design:
- **Elevation** — анимированное изменение уровня поднятия карточки при наведении и нажатии
- **Динамические тени** — мягкие тени, которые изменяются в зависимости от уровня elevation
- **Ripple-эффект** — волновой эффект при клике, имитирующий взаимодействие с материалом
- **Настраиваемые отступы** — автоматическое управление отступами для корректного отображения теней

## Возможности

- ✅ Анимированное изменение elevation при hover и press
- ✅ Настраиваемая интенсивность и резкость теней
- ✅ Ripple-эффект с кастомным цветом
- ✅ Скруглённые углы
- ✅ Автоматические отступы для предотвращения обрезания теней
- ✅ Полная поддержка Qt5 и Qt6
- ✅ Q_PROPERTY для использования в QML и стилях

## Требования

- Qt 5.x или Qt 6.x
- C++17 или выше
- CMake 3.5 или выше

## Сборка

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

Для Windows с Visual Studio:
```bash
cmake .. -G "Visual Studio 17 2022"
cmake --build . --config Release
```

## Использование

### Базовый пример

```cpp
#include "QMaterialWidget.h"
#include <QVBoxLayout>
#include <QLabel>

// Создание карточки
QMaterialWidget *card = new QMaterialWidget(this);
card->setFixedSize(300, 200);
card->setStyleSheet("background-color: white; border: 1px solid #e0e0e0;");

// Добавление содержимого
QVBoxLayout *layout = new QVBoxLayout(card);
layout->setContentsMargins(20, 20, 20, 20);

QLabel *title = new QLabel("Заголовок", card);
QLabel *text = new QLabel("Текст карточки", card);

layout->addWidget(title);
layout->addWidget(text);

// Подключение сигнала clicked
connect(card, &QMaterialWidget::clicked, []() {
    qDebug() << "Карточка нажата!";
});
```

### Настройка elevation

```cpp
// Установка уровней elevation для разных состояний
// rest, hover, pressed
card->setElevationStates(2.0, 6.0, 10.0);

// Или индивидуальная настройка
card->setElevation(4.0);
```

### Настройка теней

```cpp
// Установка отступов для теней (чтобы они не обрезались)
card->setShadowMargins(QMargins(24, 24, 24, 36));

// Настройка интенсивности тени (0.0 - прозрачная, 1.0 - полная яркость)
card->setShadowIntensity(0.5); // Лёгкая тень

// Включение/выключение теней
card->setShadowEnabled(true);
```

### Настройка ripple-эффекта

```cpp
// Включение/выключение ripple
card->setRippleEnabled(true);

// Кастомный цвет ripple
card->setRippleColor(QColor(255, 255, 255, 120)); // Белый полупрозрачный
```

### Скругление углов

```cpp
// Установка радиуса скругления
card->setCornerRadius(16.0);
```

### Полный пример с настройками

```cpp
QMaterialWidget *card = new QMaterialWidget(this);
card->setFixedSize(400, 250);

// Настройка отступов для теней
card->setShadowMargins(QMargins(28, 24, 28, 42));

// Настройка elevation
card->setElevationStates(2.0, 6.0, 12.0);

// Настройка тени
card->setShadowIntensity(0.6); // Лёгкая тень

// Скругление углов
card->setCornerRadius(16.0);

// Ripple эффект
card->setRippleColor(QColor(0, 0, 0, 60));

// Стилизация
card->setStyleSheet("background-color: #2196F3; border: none;");

// Содержимое
QVBoxLayout *layout = new QVBoxLayout(card);
layout->setContentsMargins(25, 25, 25, 25);

QLabel *title = new QLabel("Заголовок", card);
title->setStyleSheet("font-size: 20px; font-weight: bold; color: white;");
layout->addWidget(title);

QLabel *text = new QLabel("Описание карточки", card);
text->setStyleSheet("color: rgba(255, 255, 255, 0.9);");
layout->addWidget(text);
layout->addStretch();
```

## API

### Свойства (Q_PROPERTY)

- `elevation` (qreal) — текущий уровень elevation
- `elevationEnabled` (bool) — включение/выключение elevation
- `shadowEnabled` (bool) — включение/выключение теней
- `rippleEnabled` (bool) — включение/выключение ripple-эффекта
- `cornerRadius` (qreal) — радиус скругления углов
- `shadowMargins` (QMargins) — отступы для области теней
- `shadowIntensity` (qreal) — интенсивность теней (0.0-1.0)

### Методы

#### Elevation
- `qreal elevation() const` — получить текущий elevation
- `void setElevation(qreal value)` — установить elevation
- `void setElevationStates(qreal rest, qreal hover, qreal pressed)` — установить уровни для состояний
- `bool isElevationEnabled() const` — проверка включения elevation
- `void setElevationEnabled(bool on)` — включить/выключить elevation

#### Тени
- `bool isShadowEnabled() const` — проверка включения теней
- `void setShadowEnabled(bool on)` — включить/выключить тени
- `QMargins shadowMargins() const` — получить отступы для теней
- `void setShadowMargins(const QMargins &margins)` — установить отступы
- `qreal shadowIntensity() const` — получить интенсивность теней
- `void setShadowIntensity(qreal intensity)` — установить интенсивность (0.0-1.0)

#### Ripple
- `bool isRippleEnabled() const` — проверка включения ripple
- `void setRippleEnabled(bool on)` — включить/выключить ripple
- `void setRippleColor(const QColor &c)` — установить цвет ripple

#### Внешний вид
- `qreal cornerRadius() const` — получить радиус скругления
- `void setCornerRadius(qreal r)` — установить радиус скругления

#### Отступы
- `void setContentsMargins(int left, int top, int right, int bottom)` — установить пользовательские отступы
- `void setContentsMargins(const QMargins &margins)` — установить отступы через QMargins
- `QMargins contentsMargins() const` — получить пользовательские отступы (без учёта теней)
- `void getContentsMargins(int *left, int *top, int *right, int *bottom) const` — получить отступы

### Сигналы

- `void elevationChanged(qreal value)` — изменение уровня elevation
- `void clicked()` — клик по карточке

## Примеры

Проект включает демонстрационное приложение с несколькими примерами использования:
- Базовая карточка
- Карточка с настройками elevation
- Карточка без ripple-эффекта
- Карточка с кастомным цветом ripple
- Интерактивная карточка с кнопкой

Запустите приложение, чтобы увидеть все примеры в действии.

## Особенности реализации

### Отступы для теней

Виджет автоматически управляет отступами, чтобы тени не обрезались. Пользовательские отступы (через `setContentsMargins`) добавляются к отступам для теней (`shadowMargins`), создавая общие отступы виджета.

### Рисование

Виджет использует кастомное рисование в `paintEvent`:
1. Сначала рисуется тень (если включена)
2. Затем фон и границы (из styleSheet)
3. В конце рисуется ripple-эффект (если активен)

### Анимация

Изменение elevation анимируется с помощью `QPropertyAnimation` с кривой `OutCubic` и длительностью 150 мс.

## Лицензия

См. файл [LICENSE](LICENSE) для подробностей.

## Автор

Проект создан для демонстрации реализации Material Design эффектов в Qt.

## Вклад

Вклад в проект приветствуется! Пожалуйста, создавайте issues и pull requests.
