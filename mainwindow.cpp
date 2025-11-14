#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "QMaterialWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSpacerItem>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("Пример использования QMaterialWidget");
    resize(900, 700);

    // Создаём центральный виджет с вертикальным layout
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setSpacing(30);
    mainLayout->setContentsMargins(40, 40, 40, 40);

    // Заголовок
    QLabel *titleLabel = new QLabel("Примеры Material Design виджетов", this);
    titleLabel->setStyleSheet("font-size: 24px; font-weight: bold; color: #333;");
    mainLayout->addWidget(titleLabel);

    // Пример 1: Базовая карточка
    QMaterialWidget *card1 = new QMaterialWidget(this);
    card1->setFixedSize(300, 150);
    card1->setStyleSheet("background-color: white; border: 1px solid #e0e0e0;");
    
    QVBoxLayout *card1Layout = new QVBoxLayout(card1);
    card1Layout->setContentsMargins(20, 20, 20, 20);
    
    QLabel *card1Title = new QLabel("Базовая карточка", card1);
    card1Title->setStyleSheet("font-size: 18px; font-weight: bold; color: #212121;");
    QLabel *card1Text = new QLabel("Это пример базовой карточки\nс Material Design эффектами", card1);
    card1Text->setStyleSheet("color: #757575;");
    
    card1Layout->addWidget(card1Title);
    card1Layout->addWidget(card1Text);
    card1Layout->addStretch();

    // Подключаем сигнал clicked
    connect(card1, &QMaterialWidget::clicked, [this]() {
        setWindowTitle("Нажата базовая карточка!");
    });

    // Пример 2: Карточка с настройками elevation
    QMaterialWidget *card2 = new QMaterialWidget(this);
    card2->setFixedSize(300, 150);
    card2->setStyleSheet("background-color: #2196F3; border: none;");
    card2->setElevationStates(4.0, 8.0, 12.0); // rest, hover, pressed
    card2->setCornerRadius(16.0);
    
    QVBoxLayout *card2Layout = new QVBoxLayout(card2);
    card2Layout->setContentsMargins(20, 20, 20, 20);
    
    QLabel *card2Title = new QLabel("Карточка с настройками", card2);
    card2Title->setStyleSheet("font-size: 18px; font-weight: bold; color: white;");
    QLabel *card2Text = new QLabel("Увеличенные значения elevation\nи скруглённые углы", card2);
    card2Text->setStyleSheet("color: rgba(255, 255, 255, 0.9);");
    
    card2Layout->addWidget(card2Title);
    card2Layout->addWidget(card2Text);
    card2Layout->addStretch();

    // Пример 3: Карточка без ripple эффекта
    QMaterialWidget *card3 = new QMaterialWidget(this);
    card3->setFixedSize(300, 150);
    card3->setStyleSheet("background-color: #4CAF50; border: none;");
    card3->setRippleEnabled(false);
    card3->setCornerRadius(8.0);
    
    QVBoxLayout *card3Layout = new QVBoxLayout(card3);
    card3Layout->setContentsMargins(20, 20, 20, 20);
    
    QLabel *card3Title = new QLabel("Без ripple эффекта", card3);
    card3Title->setStyleSheet("font-size: 18px; font-weight: bold; color: white;");
    QLabel *card3Text = new QLabel("Ripple эффект отключён,\nно elevation работает", card3);
    card3Text->setStyleSheet("color: rgba(255, 255, 255, 0.9);");
    
    card3Layout->addWidget(card3Title);
    card3Layout->addWidget(card3Text);
    card3Layout->addStretch();

    // Пример 4: Карточка с кастомным цветом ripple
    QMaterialWidget *card4 = new QMaterialWidget(this);
    card4->setFixedSize(300, 150);
    card4->setStyleSheet("background-color: #FF9800; border: none;");
    card4->setRippleColor(QColor(255, 255, 255, 120)); // Белый ripple
    card4->setCornerRadius(20.0);
    
    QVBoxLayout *card4Layout = new QVBoxLayout(card4);
    card4Layout->setContentsMargins(20, 20, 20, 20);
    
    QLabel *card4Title = new QLabel("Кастомный ripple", card4);
    card4Title->setStyleSheet("font-size: 18px; font-weight: bold; color: white;");
    QLabel *card4Text = new QLabel("Белый ripple эффект\nна оранжевом фоне", card4);
    card4Text->setStyleSheet("color: rgba(255, 255, 255, 0.9);");
    
    card4Layout->addWidget(card4Title);
    card4Layout->addWidget(card4Text);
    card4Layout->addStretch();

    // Горизонтальный layout для карточек
    QHBoxLayout *cardsLayout = new QHBoxLayout();
    cardsLayout->setSpacing(20);
    cardsLayout->addWidget(card1);
    cardsLayout->addWidget(card2);
    cardsLayout->addStretch();
    
    QHBoxLayout *cardsLayout2 = new QHBoxLayout();
    cardsLayout2->setSpacing(20);
    cardsLayout2->addWidget(card3);
    cardsLayout2->addWidget(card4);
    cardsLayout2->addStretch();

    mainLayout->addLayout(cardsLayout);
    mainLayout->addLayout(cardsLayout2);
    mainLayout->addStretch();

    // Пример 5: Карточка с кнопкой внутри
    QMaterialWidget *card5 = new QMaterialWidget(this);
    card5->setFixedSize(400, 200);
    card5->setStyleSheet("background-color: white; border: 1px solid #e0e0e0;");
    
    QVBoxLayout *card5Layout = new QVBoxLayout(card5);
    card5Layout->setContentsMargins(25, 25, 25, 25);
    
    QLabel *card5Title = new QLabel("Интерактивная карточка", card5);
    card5Title->setStyleSheet("font-size: 20px; font-weight: bold; color: #212121;");
    
    QLabel *card5Text = new QLabel("Эта карточка содержит кнопку и реагирует на клики.\nПопробуйте навести курсор и кликнуть!", card5);
    card5Text->setStyleSheet("color: #757575; margin-top: 10px;");
    
    QPushButton *button = new QPushButton("Кнопка внутри карточки", card5);
    button->setStyleSheet(
        "QPushButton {"
        "    background-color: #2196F3;"
        "    color: white;"
        "    border: none;"
        "    padding: 10px 20px;"
        "    border-radius: 4px;"
        "    font-size: 14px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #1976D2;"
        "}"
    );
    
    card5Layout->addWidget(card5Title);
    card5Layout->addWidget(card5Text);
    card5Layout->addStretch();
    card5Layout->addWidget(button, 0, Qt::AlignRight);

    connect(card5, &QMaterialWidget::clicked, [this]() {
        setWindowTitle("Карточка с кнопкой была нажата!");
    });

    mainLayout->addWidget(card5, 0, Qt::AlignHCenter);
    mainLayout->addStretch();

    // Устанавливаем фон окна
    centralWidget->setStyleSheet("background-color: #f5f5f5;");
}

MainWindow::~MainWindow()
{
    delete ui;
}
