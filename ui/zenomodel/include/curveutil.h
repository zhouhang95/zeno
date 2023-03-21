#ifndef __CURVE_UTIL_H__
#define __CURVE_UTIL_H__

#include <zenomodel/include/modeldata.h>
#include <zenomodel/include/curvemodel.h>

namespace curve_util
{
	enum ItemType
	{
		ITEM_NODE,
		ITEM_LEFTHANDLE,
		ITEM_RIGHTHANDLE,
		ITEM_CURVE,
	};

	enum ItemStatus
	{
		ITEM_UNTOGGLED,
		ITEM_TOGGLED,
		ITEM_SELECTED,
	};

	enum ROLE_CURVE
	{
		ROLE_ItemType = Qt::UserRole + 1,
		ROLE_ItemObjId,
		ROLE_ItemBelongTo,
		ROLE_ItemPos,
		ROLE_ItemStatus,
		ROLE_MouseClicked,
		ROLE_CurveLeftNode,
		ROLE_CurveRightNode
	};

	enum CurveGVType
	{
		CURVE_NODE = QGraphicsItem::UserType + 1,
		CURVE_HANDLE
	};

	QRectF fitInRange(CURVE_RANGE rg, const QMargins& margins);
	QRectF initGridSize(const QSize& sz, const QMargins& margins);
	QModelIndex findUniqueItem(QAbstractItemModel* pModel, int role, QVariant value);
	QPair<int, int> numframes(qreal scaleX, qreal scaleY);
	CurveModel* deflModel(QObject* parent);
	CURVES_DATA deflCurves();
}

#endif