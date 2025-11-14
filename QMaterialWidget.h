#pragma once

#include <QWidget>
#include <QColor>
#include <QPropertyAnimation>
#include <QTimer>
#include <QPainterPath>
#include <QtGlobal>

class QMaterialWidget : public QWidget
{
    Q_OBJECT

    Q_PROPERTY(qreal elevation READ elevation WRITE setElevation NOTIFY elevationChanged)
    Q_PROPERTY(bool elevationEnabled READ isElevationEnabled WRITE setElevationEnabled)
    Q_PROPERTY(bool shadowEnabled READ isShadowEnabled WRITE setShadowEnabled)
    Q_PROPERTY(bool rippleEnabled READ isRippleEnabled WRITE setRippleEnabled)
    Q_PROPERTY(qreal cornerRadius READ cornerRadius WRITE setCornerRadius)

public:
    explicit QMaterialWidget(QWidget *parent = nullptr);

    // elevation value used in paint (animated)
    qreal elevation() const { return m_elevation; }
    void setElevation(qreal value);

    bool isElevationEnabled() const { return m_elevationEnabled; }
    void setElevationEnabled(bool on);

    bool isShadowEnabled() const { return m_shadowEnabled; }
    void setShadowEnabled(bool on);

    bool isRippleEnabled() const { return m_rippleEnabled; }
    void setRippleEnabled(bool on);

    qreal cornerRadius() const { return m_cornerRadius; }
    void setCornerRadius(qreal r);

    // Настройка уровней elevation
    void setElevationStates(qreal rest, qreal hover, qreal pressed);

    // Цвет ripple
    void setRippleColor(const QColor &c) { m_rippleColor = c; }

signals:
    void elevationChanged(qreal value);
    void clicked(); // удобный сигнал "карточка нажата"

protected:
    void paintEvent(QPaintEvent *event) override;

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    void enterEvent(QEnterEvent *event) override;
#else
    void enterEvent(QEvent *event) override;
#endif

    void leaveEvent(QEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private slots:
    void updateRipple();

private:
    void startElevationAnimation(qreal target);
    QPainterPath cardClipPath(const QRectF &r) const;
    void paintShadow(QPainter &p, const QRectF &cardRect);

    // Включатели
    bool m_elevationEnabled;
    bool m_shadowEnabled;
    bool m_rippleEnabled;

    // Текущий elevation (анимируемое свойство)
    qreal m_elevation;
    qreal m_cornerRadius;

    // Состояния elevation
    qreal m_restElevation;
    qreal m_hoverElevation;
    qreal m_pressedElevation;

    QPropertyAnimation *m_elevationAnim;

    // Ripple
    QTimer  m_rippleTimer;
    QPointF m_rippleCenter;
    qreal   m_rippleRadius;
    qreal   m_rippleMaxRadius;
    qreal   m_rippleOpacity;
    QColor  m_rippleColor;
    int     m_rippleDurationMs;
    bool    m_mousePressedInside;
};
