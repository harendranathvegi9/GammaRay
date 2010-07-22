#include "objecttreemodel.h"

#include <kdebug.h>

using namespace Endoscope;

ObjectTreeModel::ObjectTreeModel(QObject* parent): ObjectModelBase< QAbstractItemModel >(parent)
{
}

void ObjectTreeModel::objectAdded(QObject* obj)
{
  QVector<QObject*> &children = m_parentChildMap[ obj->parent() ];
  beginInsertRows( indexForObject( obj->parent() ), children.size(), children.size() );
  children.push_back( obj );
  m_childParentMap.insert( obj, obj->parent() );
  endInsertRows();
}

void ObjectTreeModel::objectRemoved(QObject* obj)
{
  QObject *parentObj = m_childParentMap[ obj ];
  const QModelIndex parentIndex = indexForObject( parentObj );
  QVector<QObject*> &children = m_parentChildMap[ parentObj ];
  const int index = children.indexOf( obj );
  if ( index < 0 || index >= children.size() )
    return;
  beginRemoveRows( parentIndex, index, index );
  children.remove( index );
  m_childParentMap.remove( obj );
  m_parentChildMap.remove( obj );
  endRemoveRows();
}

QVariant ObjectTreeModel::data(const QModelIndex& index, int role) const
{
  QObject *obj = reinterpret_cast<QObject*>( index.internalPointer() );
  if ( obj )
    return dataForObject( obj, index, role );
  return QVariant();
}

int ObjectTreeModel::rowCount(const QModelIndex& parent) const
{
  QObject *parentObj = reinterpret_cast<QObject*>( parent.internalPointer() );
  return m_parentChildMap.value( parentObj ).size();
}

QModelIndex ObjectTreeModel::parent(const QModelIndex& child) const
{
  QObject *childObj = reinterpret_cast<QObject*>( child.internalPointer() );
  return indexForObject( m_childParentMap.value( childObj ) );
}

QModelIndex ObjectTreeModel::index(int row, int column, const QModelIndex& parent) const
{
  QObject *parentObj = reinterpret_cast<QObject*>( parent.internalPointer() );
  const QVector<QObject*> children = m_parentChildMap.value( parentObj );
  if ( row < 0 || row >= children.size() )
    return QModelIndex();
  return createIndex( row, column, children.at( row ) );
}

QModelIndex ObjectTreeModel::indexForObject( QObject* object ) const
{
  if ( !object )
    return QModelIndex();
  QObject *parent = m_childParentMap.value( object );
  const QModelIndex parentIndex = indexForObject( parent );
  int row = m_parentChildMap[ parent ].indexOf( object );
  return index( row, 0, parentIndex );
}

#include "objecttreemodel.moc"