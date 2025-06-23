#pragma once
#include <QPointF>
#include <QRectF>
#include <vector>
#include <cmath>

// A namespace to hold our pathfinding logic and data structures
namespace Pathfinder {

    // Represents a single cell in our virtual navigation grid
    struct GridCell {
        int x, y;
        bool isObstacle = false;

        // A* algorithm costs
        float gCost = INFINITY; // Cost from start node
        float hCost = INFINITY; // Heuristic cost to end node

        GridCell* parent = nullptr;

        float fCost() const { return gCost + hCost; }
    };

    // The main pathfinder class
    class AStarPathfinder
    {
    public:
        AStarPathfinder(const QRectF& bounds, float cellSize);
        void addObstacle(const QRectF& obstacle);
        std::vector<QPointF> findPath(const QPointF& start, const QPointF& end);

    private:
        QPoint worldToGrid(const QPointF& point) const;
        QPointF gridToWorld(const QPoint& point) const;
        float getDistance(const GridCell* a, const GridCell* b) const;
        std::vector<GridCell*> getNeighbours(const GridCell* cell) const;
        std::vector<QPointF> retracePath(const GridCell* startCell, const GridCell* endCell) const;
        std::vector<QPointF> simplifyPath(const std::vector<QPointF>& path) const;

        float m_cellSize;
        QRectF m_bounds;
        int m_gridWidth;
        int m_gridHeight;
        std::vector<std::vector<GridCell>> m_grid;
    };

} // namespace Pathfinder