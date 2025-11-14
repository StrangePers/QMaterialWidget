#include "QMaterialWidget.h"

#include <QPainter>
#include <QStyleOption>
#include <QMouseEvent>
#include <QEasingCurve>
#include <QStyle>

QMaterialWidget::QMaterialWidget(QWidget *parent)
    : QWidget(parent),
      m_elevationEnabled(true),
      m_shadowEnabled(true),
      m_rippleEnabled(true),
      m_elevation(2.0),
      m_cornerRadius(12.0),
      m_restElevation(2.0),
      m_hoverElevation(6.0),
      m_pressedElevation(10.0),
      m_elevationAnim(new QPropertyAnimation(this, "elevation", this)),
      m_rippleRadius(0.0),
      m_rippleMaxRadius(0.0),
      m_rippleOpacity(0.0),
      m_rippleColor(0, 0, 0, 80),
      m_rippleDurationMs(250),
      m_mousePressedInside(false),
      m_shadowMargins(24, 24, 24, 36),
      m_userContentsMargins(0, 0, 0, 0)
{
    // Не используем WA_StyledBackground, чтобы фон не рисовался под тенью
    // Вместо этого будем рисовать фон вручную только внутри области карточки
    setMouseTracking(true);
    setAutoFillBackground(false);
    applyEffectiveContentsMargins();

    // Анимация elevation
    m_elevationAnim->setDuration(150);
    m_elevationAnim->setEasingCurve(QEasingCurve::OutCubic);

    // Ripple таймер
    m_rippleTimer.setInterval(16); // ~60 FPS
    connect(&m_rippleTimer, &QTimer::timeout,
            this, &QMaterialWidget::updateRipple);
}

void QMaterialWidget::setElevation(qreal value)
{
    if (qFuzzyCompare(m_elevation, value))
        return;

    m_elevation = value;
    emit elevationChanged(m_elevation);
    update();
}

void QMaterialWidget::setElevationEnabled(bool on)
{
    if (m_elevationEnabled == on)
        return;

    m_elevationEnabled = on;

    if (!m_elevationEnabled) {
        m_elevationAnim->stop();
        m_elevation = 0.0;
        update();
    } else {
        // вернёмся к базовому состоянию
        startElevationAnimation(m_restElevation);
    }
}

void QMaterialWidget::setShadowEnabled(bool on)
{
    if (m_shadowEnabled == on)
        return;

    m_shadowEnabled = on;
    update();
}

void QMaterialWidget::setRippleEnabled(bool on)
{
    if (m_rippleEnabled == on)
        return;

    m_rippleEnabled = on;
    if (!m_rippleEnabled) {
        m_rippleTimer.stop();
        m_rippleOpacity = 0.0;
        update();
    }
}

void QMaterialWidget::setCornerRadius(qreal r)
{
    if (qFuzzyCompare(m_cornerRadius, r))
        return;

    m_cornerRadius = r;
    update();
}

void QMaterialWidget::setElevationStates(qreal rest, qreal hover, qreal pressed)
{
    m_restElevation   = rest;
    m_hoverElevation  = hover;
    m_pressedElevation = pressed;

    if (!m_elevationEnabled) {
        m_elevation = 0.0;
        update();
    }
}

void QMaterialWidget::setShadowMargins(const QMargins &margins)
{
    if (m_shadowMargins == margins)
        return;

    m_shadowMargins = margins;
    applyEffectiveContentsMargins();
    update();
}

void QMaterialWidget::setContentsMargins(int left, int top, int right, int bottom)
{
    setContentsMargins(QMargins(left, top, right, bottom));
}

void QMaterialWidget::setContentsMargins(const QMargins &margins)
{
    if (m_userContentsMargins == margins)
        return;

    m_userContentsMargins = margins;
    applyEffectiveContentsMargins();
}

void QMaterialWidget::getContentsMargins(int *left, int *top, int *right, int *bottom) const
{
    if (left) {
        *left = m_userContentsMargins.left();
    }
    if (top) {
        *top = m_userContentsMargins.top();
    }
    if (right) {
        *right = m_userContentsMargins.right();
    }
    if (bottom) {
        *bottom = m_userContentsMargins.bottom();
    }
}

QMargins QMaterialWidget::contentsMargins() const
{
    return m_userContentsMargins;
}

void QMaterialWidget::applyEffectiveContentsMargins()
{
    QWidget::setContentsMargins(totalContentsMargins());
}

QMargins QMaterialWidget::totalContentsMargins() const
{
    return QMargins(
        m_userContentsMargins.left() + m_shadowMargins.left(),
        m_userContentsMargins.top() + m_shadowMargins.top(),
        m_userContentsMargins.right() + m_shadowMargins.right(),
        m_userContentsMargins.bottom() + m_shadowMargins.bottom());
}

QRectF QMaterialWidget::effectiveCardRect() const
{
    QRectF r = rect();
    r.adjust(m_shadowMargins.left(),
             m_shadowMargins.top(),
             -m_shadowMargins.right(),
             -m_shadowMargins.bottom());

    if (r.width() <= 0 || r.height() <= 0) {
        return QRectF(rect());
    }

    return r;
}

void QMaterialWidget::startElevationAnimation(qreal target)
{
    if (!m_elevationEnabled) {
        return;
    }

    m_elevationAnim->stop();
    m_elevationAnim->setStartValue(m_elevation);
    m_elevationAnim->setEndValue(target);
    m_elevationAnim->start();
}

QPainterPath QMaterialWidget::cardClipPath(const QRectF &r) const
{
    QPainterPath path;
    path.addRoundedRect(r, m_cornerRadius, m_cornerRadius);
    return path;
}

void QMaterialWidget::paintShadow(QPainter &p, const QRectF &cardRect)
{
    if (!m_shadowEnabled || !m_elevationEnabled || m_elevation <= 0.0)
        return;

    p.save();

    // Немного опустим тень вниз, имитируя "поднятие"
    const qreal yOffset = m_elevation * 0.4;
    const qreal blurRadius = 2.0 + m_elevation * 1.5;
    const int steps = 8;

    QColor baseColor(0, 0, 0);
    baseColor.setAlpha(qBound(20, int(25 + m_elevation * 6), 130));

    for (int i = 0; i < steps; ++i) {
        const qreal t = qreal(i + 1) / steps;
        const qreal grow = blurRadius * t;

        QRectF r = cardRect.adjusted(-grow, -grow, grow, grow);
        r.translate(0, yOffset); // смещение вниз

        QColor c = baseColor;
        c.setAlphaF(baseColor.alphaF() * (1.0 - t));

        QPainterPath path;
        path.addRoundedRect(r, m_cornerRadius + grow, m_cornerRadius + grow);

        p.setPen(Qt::NoPen);
        p.setBrush(c);
        p.drawPath(path);
    }

    p.restore();
}

void QMaterialWidget::paintBackground(QPainter &p, const QRectF &cardRect)
{
    // Рисуем фон из QSS только внутри области карточки
    // Используем QStyleOption для правильного применения стилей
    QStyleOption opt;
    opt.initFrom(this);
    opt.rect = cardRect.toRect();
    
    // Временно включаем WA_StyledBackground для правильного чтения QSS стилей
    // Но рисуем только в области карточки благодаря clipping в paintEvent
    bool hadStyledBackground = testAttribute(Qt::WA_StyledBackground);
    setAttribute(Qt::WA_StyledBackground, true);
    
    // Рисуем фон и border через стиль
    // Это правильно применит все стили из QSS (background-color, border и т.д.)
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
    
    // Восстанавливаем исходное состояние
    setAttribute(Qt::WA_StyledBackground, hadStyledBackground);
}

void QMaterialWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);

    // Немного отступим от краёв, чтобы тень и скругление не обрезались
    QRectF cardRect = effectiveCardRect();
    if (cardRect.isEmpty())
        return;

    // 1) Тень (под карточкой)
    paintShadow(p, cardRect);

    // 2) Фон и бордеры из styleSheet / QStyle (только внутри области карточки)
    p.save();
    p.setClipPath(cardClipPath(cardRect));
    paintBackground(p, cardRect);
    p.restore();

    // Дальше Qt сам нарисует детей (QLabel, QLayout и т.п.)

    // 3) Ripple-эффект поверх фона (под детьми с непрозрачным фоном)
    if (m_rippleEnabled && m_rippleOpacity > 0.0) {
        p.save();
        p.setClipPath(cardClipPath(cardRect));

        QColor c = m_rippleColor;
        c.setAlphaF(c.alphaF() * m_rippleOpacity);

        p.setPen(Qt::NoPen);
        p.setBrush(c);
        p.drawEllipse(m_rippleCenter, m_rippleRadius, m_rippleRadius);

        p.restore();
    }
}

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
void QMaterialWidget::enterEvent(QEnterEvent *event)
{
    QWidget::enterEvent(event);
    if (m_elevationEnabled) {
        startElevationAnimation(m_hoverElevation);
    }
}
#else
void QMaterialWidget::enterEvent(QEvent *event)
{
    QWidget::enterEvent(event);
    if (m_elevationEnabled) {
        startElevationAnimation(m_hoverElevation);
    }
}
#endif

void QMaterialWidget::leaveEvent(QEvent *event)
{
    QWidget::leaveEvent(event);
    if (m_elevationEnabled) {
        startElevationAnimation(m_restElevation);
    }
}

void QMaterialWidget::mousePressEvent(QMouseEvent *event)
{
    const QRectF card = effectiveCardRect();
    m_mousePressedInside = card.contains(event->pos());

    if (m_elevationEnabled) {
        startElevationAnimation(m_pressedElevation);
    }

    if (m_rippleEnabled && m_mousePressedInside) {
        // Начинаем ripple
        m_rippleCenter = event->pos();
        m_rippleRadius = 0.0;
        m_rippleOpacity = 1.0;

        // Максимальный радиус — до самого дальнего угла
        const QPointF topLeft = card.topLeft();
        const QPointF topRight = card.topRight();
        const QPointF bottomLeft = card.bottomLeft();
        const QPointF bottomRight = card.bottomRight();

        qreal d1 = QLineF(m_rippleCenter, topLeft).length();
        qreal d2 = QLineF(m_rippleCenter, topRight).length();
        qreal d3 = QLineF(m_rippleCenter, bottomLeft).length();
        qreal d4 = QLineF(m_rippleCenter, bottomRight).length();

        m_rippleMaxRadius = qMax(qMax(d1, d2), qMax(d3, d4));

        if (!m_rippleTimer.isActive())
            m_rippleTimer.start();
    }

    QWidget::mousePressEvent(event);
}

void QMaterialWidget::mouseReleaseEvent(QMouseEvent *event)
{
    const QRectF card = effectiveCardRect();
    const bool wasPressedInside = m_mousePressedInside;
    m_mousePressedInside = false;

    if (m_elevationEnabled) {
        // Если мышь всё ещё над виджетом — вернёмся к hover
        if (card.contains(event->pos())) {
            startElevationAnimation(m_hoverElevation);
        } else {
            startElevationAnimation(m_restElevation);
        }
    }

    if (wasPressedInside && card.contains(event->pos())) {
        emit clicked();
    }

    QWidget::mouseReleaseEvent(event);
}

void QMaterialWidget::updateRipple()
{
    if (!m_rippleEnabled) {
        m_rippleTimer.stop();
        return;
    }

    if (m_rippleOpacity <= 0.0) {
        m_rippleTimer.stop();
        m_rippleOpacity = 0.0;
        update();
        return;
    }

    const qreal dt = qreal(m_rippleTimer.interval()) / m_rippleDurationMs;
    m_rippleRadius += m_rippleMaxRadius * dt;
    m_rippleOpacity -= dt;

    if (m_rippleRadius >= m_rippleMaxRadius) {
        m_rippleOpacity -= dt * 2.0; // чуть быстрее затухаем в конце
    }

    if (m_rippleOpacity < 0.0) {
        m_rippleOpacity = 0.0;
    }

    update();
}
