// Copyright (c) 2019 The PIVX developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "qt/furabstractlistitemdelegate.h"

FurAbstractListItemDelegate::FurAbstractListItemDelegate(int _rowHeight, FurListRow<> *_row, QObject *parent) :
    QAbstractItemDelegate(parent), rowHeight(_rowHeight), row(_row){}

void FurAbstractListItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                  const QModelIndex &index ) const
{
    painter->save();

    // Status
    bool isStateSelected = option.state & QStyle::State_Selected;
    bool isStateHovered = option.state & QStyle::State_MouseOver;

    QRect selectedRect = option.rect;

	QFont font = painter->font();
	QPen pen(QColor(Qt::white), 2);
	font.setPixelSize(10);

	painter->setRenderHint(QPainter::Antialiasing);
	painter->setFont(font);
	painter->setPen(pen);
	painter->setBrush(QColor(26, 29, 49, 127));
	QMargins margin(0, 4, 0, 4);
	selectedRect = selectedRect.marginsRemoved(margin);
	painter->drawRoundedRect(selectedRect, 20.0, 20.0);

	//painter->fillPath(path, QColor("transparent"));

    //painter->fillRect(selectedRect, this->row->rectColor(isStateHovered, isStateSelected));

    painter->translate(option.rect.topLeft());
    QWidget *row = this->row->createHolder(index.row());

    this->row->init(row, index, isStateHovered, isStateSelected);
    row->setAttribute(Qt::WA_DontShowOnScreen, true);
    row->setGeometry(option.rect);
    row->resize(option.rect.width(),option.rect.height());
    row->setStyleSheet("color:white; font-size:14px; text-align:right;");
    row->render(painter, QPoint(), QRegion(), QWidget::DrawChildren );

    painter->restore();
}

FurListRow<>* FurAbstractListItemDelegate::getRowFactory(){
    return this->row;
}

QSize FurAbstractListItemDelegate::sizeHint(const QStyleOptionViewItem&, const QModelIndex&) const
{
    return QSize(rowHeight, rowHeight);
}
