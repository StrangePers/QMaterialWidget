#include "QMaterialWidget.h"

#include <QPainter>
#include <QStyleOption>
#include <QMouseEvent>
#include <QEasingCurve>

MaterialCardWidget::MaterialCardWidget(QWidget *parent)
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
      m_mousePressedInside(false)
{
    // Нужен для корректной отрисовки фона из styleSheet
    setAttribute(Qt::WA_StyledBackground, true);

    setMouseTracking(true);
    setAutoFillBackground(false);

    // Анимация elevation
    m_elevationAnim->setDuration(150);
    m_elevationAnim->setEasingCurve(QEasingCurve::OutCubic);

    // Ripple таймер
    m_rippleTimer.setInterval(16); // ~60 FPS
    connect(&m_rippleTimer, &QTimer::timeout,
            this, &MaterialCardWidget::updateRipple);
}

void MaterialCardWidget::setElevation(qreal value)
{
    if (qFuzzyCompare(m_elevation, value))
        return;

    m_elevation = value;
    emit elevationChanged(m_elevation);
    update();
}

void MaterialCardWidget::setElevationEnabled(bool on)
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

void MaterialCardWidget::setShadowEnabled(bool on)
{
    if (m_shadowEnabled == on)
        return;

    m_shadowEnabled = on;
    update();
}

void MaterialCardWidget::setRippleEnabled(bool on)
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

void MaterialCardWidget::setCornerRadius(qreal r)
{
    if (qFuzzyCompare(m_cornerRadius, r))
        return;

    m_cornerRadius = r;
    update();
}

void MaterialCardWidget::setElevationStates(qreal rest, qreal hover, qreal pressed)
{
    m_restElevation   = rest;
    m_hoverElevation  = hover;
    m_pressedElevation = pressed;

    if (!m_elevationEnabled) {
        m_elevation = 0.0;
        update();
    }
}

void MaterialCardWidget::startElevationAnimation(qreal target)
{
    if (!m_elevationEnabled) {
        return;
    }

    m_elevationAnim->stop();
    m_elevationAnim->setStartValue(m_elevation);
    m_elevationAnim->setEndValue(target);
    m_elevationAnim->start();
}

QPainterPath MaterialCardWidget::cardClipPath(const QRectF &r) const
{
    QPainterPath path;
    path.addRoundedRect(r, m_cornerRadius, m_cornerRadius);
    return path;
}

void MaterialCardWidget::paintShadow(QPainter &p, const QRectF &cardRect)
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

void MaterialCardWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);

    // Немного отступим от краёв, чтобы тень и скругление не обрезались
    QRectF cardRect = rect().adjusted(2, 2, -2, -2);

    // 1) Тень (под карточкой)
    paintShadow(p, cardRect);

    // 2) Фон и бордеры из styleSheet / QStyle
    {
        QStyleOption opt;
        opt.initFrom(this);

        p.save();
        p.setClipPath(cardClipPath(cardRect));
        style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
        p.restore();
    }

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
void MaterialCardWidget::enterEvent(QEnterEvent *event)
{
    QWidget::enterEvent(event);
    if (m_elevationEnabled) {
        startElevationAnimation(m_hoverElevation);
    }
}
#else
void MaterialCardWidget::enterEvent(QEvent *event)
{
    QWidget::enterEvent(event);
    if (m_elevationEnabled) {
        startElevationAnimation(m_hoverElevation);
    }
}
#endif

void MaterialCardWidget::leaveEvent(QEvent *event)
{
    QWidget::leaveEvent(event);
    if (m_elevationEnabled) {
        startElevationAnimation(m_restElevation);
    }
}

void MaterialCardWidget::mousePressEvent(QMouseEvent *event)
{
    m_mousePressedInside = rect().contains(event->pos());

    if (m_elevationEnabled) {
        startElevationAnimation(m_pressedElevation);
    }

    if (m_rippleEnabled && m_mousePressedInside) {
        // Начинаем ripple
        m_rippleCenter = event->pos();
        m_rippleRadius = 0.0;
        m_rippleOpacity = 1.0;

        // Максимальный радиус — до самого дальнего угла
        const QRectF r = rect();
        const QPointF topLeft = r.topLeft();
        const QPointF topRight = r.topRight();
        const QPointF bottomLeft = r.bottomLeft();
        const QPointF bottomRight = r.bottomRight();

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

void MaterialCardWidget::mouseReleaseEvent(QMouseEvent *event)
{
    const bool wasPressedInside = m_mousePressedInside;
    m_mousePressedInside = false;

    if (m_elevationEnabled) {
        // Если мышь всё ещё над виджетом — вернёмся к hover
        if (rect().contains(event->pos())) {
            startElevationAnimation(m_hoverElevation);
        } else {
            startElevationAnimation(m_restElevation);
        }
    }

    if (wasPressedInside && rect().contains(event->pos())) {
        emit clicked();
    }

    QWidget::mouseReleaseEvent(event);
}

void MaterialCardWidget::updateRipple()
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
