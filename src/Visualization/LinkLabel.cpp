#include <QToolTip>
#include <QTimer>

#include "LinkLabel.hpp"

LinkLabel::LinkLabel(const QString& text, QWidget* parent)
    : QLabel(text,parent)
{
    setStyleSheet("QLabel { color : blue; }");
    _down = false;
    _enabled = true;
    _checkable = false;
    _type = true;
    _tooltip = text;
    _hyperlink = true;
    setMouseTracking(true);
}

LinkLabel::LinkLabel(const QString& text, QString tooltip, QWidget* parent)
    : QLabel(text,parent)
{
    setStyleSheet("QLabel { color : blue; }");
    _down = false;
    _enabled = true;
    _checkable = false;
    _type = true;
    _tooltip = tooltip;
    _hyperlink = true;
    setMouseTracking(true);
}

LinkLabel::LinkLabel(const QPixmap& upImg, const QPixmap& downImg, QWidget* parent)
    : QLabel(parent)
{
    _upImg = upImg;
    _downImg = downImg;
    setPixmap(_upImg);
    setFixedSize(upImg.size());
    _down = false;
    _enabled = true;
    _checkable = false;
    _type = false;
    _tooltip = QString();
    _hyperlink = true;
    setMouseTracking(true);
}

LinkLabel::LinkLabel(const QPixmap& upImg, const QPixmap& downImg, QString text, QWidget* parent)
    : QLabel(parent)
{
    _upImg = upImg;
    _downImg = downImg;
    setPixmap(_upImg);
    setFixedSize(upImg.size());
    _down = false;
    _enabled = true;
    _checkable = false;
    _type = false;
    _tooltip = text;
    _hyperlink = true;
    setMouseTracking(true);
}

void LinkLabel::setChecked(bool checked)
{
    _down = checked;
    checked ? setPixmap(_downImg) : setPixmap(_upImg);
    emit toggled(_down);
}

void LinkLabel::setState(bool up)
{
    _down = !up;
    _down ? setPixmap(_downImg) : setPixmap(_upImg);
}

void LinkLabel::enableHyperlinkStyle(bool enable)
{
    _hyperlink = enable;
    if(isText()) {
        if(_hyperlink)
            setStyleSheet("QLabel { color : blue; text-decoration: underline }");
        else
            setStyleSheet("");
    }
}

void LinkLabel::newImage(const QPixmap& upImg, const QPixmap& downImg) {
    // If it's not an image label, do nothing.
    // May want to add support for transitions between types in the future
    if(_type)
        return;
    _upImg = upImg;
    _downImg = downImg;
    if(_down)
        setPixmap(_downImg);
    else
        setPixmap(_upImg);
}

void LinkLabel::newImage(const QPixmap& upImg, const QPixmap& downImg, QString text) {
    _tooltip = text;
    newImage(upImg, downImg);
}

/** Search for non-standard events (like tooltip stuff). */
bool LinkLabel::event(QEvent *event)
{
    // Specifically handle tooltip events.
    if(event->type() == QEvent::ToolTip) {
        toolTipEvent(static_cast<QHelpEvent*>(event));
        return true;
    }
    return QLabel::event(event);
}

void LinkLabel::scaleToHeight(int height) {
    if(!_type) {
        _upImg = _upImg.scaledToHeight(height, Qt::SmoothTransformation);
        _downImg = _downImg.scaledToHeight(height, Qt::SmoothTransformation);
        setFixedSize(_upImg.size());
        if(!_down)
            setPixmap(_upImg);
        else
            setPixmap(_downImg);
    }
}

void LinkLabel::scaleToWidth(int width) {
    if(!_type) {
        _upImg = _upImg.scaledToWidth(width, Qt::SmoothTransformation);
        _downImg = _downImg.scaledToWidth(width, Qt::SmoothTransformation);
        setFixedSize(_upImg.size());
        if(!_down)
            setPixmap(_upImg);
        else
            setPixmap(_downImg);
    }
}

void LinkLabel::mousePressEvent(QMouseEvent* event)
{
    if((event->button() == Qt::LeftButton) && isEnabled()) {
        if(isText())
            setStyleSheet("QLabel { color : blue; text-decoration: underline }");
        else
            setPixmap(_downImg);
    if(!isCheckable())
        _down = true;
    }

    if (!_hyperlink) setStyleSheet("");
    QLabel::mousePressEvent(event);
}

void LinkLabel::mouseReleaseEvent(QMouseEvent* event)
{
    if((event->button() == Qt::LeftButton) && (isEnabled())) {
        if(isCheckable()) {
            if(isText()) {
                if(_down)
                    setStyleSheet("QLabel { color : blue; }");
            } else {
                if(_down)
                    setPixmap(_upImg);
            }
            _down = !_down;
            emit toggled(_down);
        } else {
            if(isText())
                setStyleSheet("QLabel { color : blue; }");
            else
                setPixmap(_upImg);
            _down = false;
            emit clicked();
            emit clicked(this);
        }
    }
    if (!_hyperlink) setStyleSheet("");

    QLabel::mouseReleaseEvent(event);
}

void LinkLabel::enterEvent(QEvent*)
{
    if(isText() && isEnabled() && _hyperlink)
        setStyleSheet("QLabel { color : blue; text-decoration: underline }");
}

void LinkLabel::leaveEvent(QEvent*)
{
    if(isText() && isEnabled() && _hyperlink)
        setStyleSheet("QLabel { color : blue; }");
}

void LinkLabel::toolTipEvent(QHelpEvent* event)
{
    event->accept();
    if(_tooltip.isEmpty()) {
        QToolTip::hideText();
    } else {
        QToolTip::showText(event->globalPos(), _tooltip, this);
    }
}
