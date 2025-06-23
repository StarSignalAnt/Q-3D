#include "Pathfinder.h"
#include <vector>
#include <algorithm>

namespace Pathfinder {

    // Constructor: Creates the grid based on the canvas size
    AStarPathfinder::AStarPathfinder(const QRectF& bounds, float cellSize)
        : m_cellSize(cellSize), m_bounds(bounds)
    {
        m_gridWidth = static_cast<int>(std::ceil(bounds.width() / m_cellSize));
        m_gridHeight = static_cast<int>(std::ceil(bounds.height() / m_cellSize));

        m_grid.resize(m_gridWidth);
        for (int x = 0; x < m_gridWidth; ++x) {
            m_grid[x].resize(m_gridHeight);
            for (int y = 0; y < m_gridHeight; ++y) {
                m_grid[x][y].x = x;
                m_grid[x][y].y = y;
            }
        }
    }

    // Marks all cells within a given rectangle as unwalkable obstacles
    void AStarPathfinder::addObstacle(const QRectF& obstacle) {
        QPoint start = worldToGrid(obstacle.topLeft());
        QPoint end = worldToGrid(obstacle.bottomRight());

        for (int x = start.x(); x <= end.x(); ++x) {
            for (int y = start.y(); y <= end.y(); ++y) {
                if (x >= 0 && x < m_gridWidth && y >= 0 && y < m_gridHeight) {
                    m_grid[x][y].isObstacle = true;
                }
            }
        }
    }

    // The core A* algorithm implementation
    std::vector<QPointF> AStarPathfinder::findPath(const QPointF& start, const QPointF& end) {
        QPoint startGridPos = worldToGrid(start);
        QPoint endGridPos = worldToGrid(end);

        GridCell* startCell = &m_grid[startGridPos.x()][startGridPos.y()];
        GridCell* endCell = &m_grid[endGridPos.x()][endGridPos.y()];

        std::vector<GridCell*> openList;
        std::vector<GridCell*> closedList;

        openList.push_back(startCell);
        startCell->gCost = 0;
        startCell->hCost = getDistance(startCell, endCell);

        while (!openList.empty()) {
            // Get the cell with the lowest F cost from the open list
            GridCell* currentCell = openList[0];
            for (size_t i = 1; i < openList.size(); ++i) {
                if (openList[i]->fCost() < currentCell->fCost() ||
                    (openList[i]->fCost() == currentCell->fCost() && openList[i]->hCost < currentCell->hCost)) {
                    currentCell = openList[i];
                }
            }

            // Move current cell from open to closed list
            openList.erase(std::remove(openList.begin(), openList.end(), currentCell), openList.end());
            closedList.push_back(currentCell);

            // If we've reached the end, retrace and return the path
            if (currentCell == endCell) {
                return simplifyPath(retracePath(startCell, endCell));
            }

            // Process neighbours
            for (GridCell* neighbour : getNeighbours(currentCell)) {
                if (neighbour->isObstacle || std::find(closedList.begin(), closedList.end(), neighbour) != closedList.end()) {
                    continue;
                }

                float newGCost = currentCell->gCost + getDistance(currentCell, neighbour);
                if (newGCost < neighbour->gCost || std::find(openList.begin(), openList.end(), neighbour) == openList.end()) {
                    neighbour->gCost = newGCost;
                    neighbour->hCost = getDistance(neighbour, endCell);
                    neighbour->parent = currentCell;

                    if (std::find(openList.begin(), openList.end(), neighbour) == openList.end()) {
                        openList.push_back(neighbour);
                    }
                }
            }
        }

        return {}; // Return empty path if no solution found
    }

    // Helper functions for the Pathfinder
    QPoint AStarPathfinder::worldToGrid(const QPointF& point) const {
        int x = static_cast<int>((point.x() - m_bounds.x()) / m_cellSize);
        int y = static_cast<int>((point.y() - m_bounds.y()) / m_cellSize);
        return QPoint(x, y);
    }

    QPointF AStarPathfinder::gridToWorld(const QPoint& point) const {
        float x = point.x() * m_cellSize + m_bounds.x() + m_cellSize / 2.0f;
        float y = point.y() * m_cellSize + m_bounds.y() + m_cellSize / 2.0f;
        return QPointF(x, y);
    }

    float AStarPathfinder::getDistance(const GridCell* a, const GridCell* b) const {
        return std::sqrt(std::pow(a->x - b->x, 2) + std::pow(a->y - b->y, 2));
    }

    std::vector<GridCell*> AStarPathfinder::getNeighbours(const GridCell* cell) const {
        std::vector<GridCell*> neighbours;
        for (int x = -1; x <= 1; ++x) {
            for (int y = -1; y <= 1; ++y) {
                if (x == 0 && y == 0) continue;
                int checkX = cell->x + x;
                int checkY = cell->y + y;
                if (checkX >= 0 && checkX < m_gridWidth && checkY >= 0 && checkY < m_gridHeight) {
                    neighbours.push_back(const_cast<GridCell*>(&m_grid[checkX][checkY]));
                }
            }
        }
        return neighbours;
    }

    std::vector<QPointF> AStarPathfinder::retracePath(const GridCell* startCell, const GridCell* endCell) const {
        std::vector<QPointF> path;
        const GridCell* currentCell = endCell;
        while (currentCell != startCell) {
            path.push_back(gridToWorld(QPoint(currentCell->x, currentCell->y)));
            currentCell = currentCell->parent;
        }
        path.push_back(gridToWorld(QPoint(startCell->x, startCell->y)));
        std::reverse(path.begin(), path.end());
        return path;
    }

    // Simplifies a path by removing points that are in a straight line
    std::vector<QPointF> AStarPathfinder::simplifyPath(const std::vector<QPointF>& path) const {
        if (path.size() < 3) return path;
        std::vector<QPointF> simplifiedPath;
        simplifiedPath.push_back(path[0]);
        for (size_t i = 1; i < path.size() - 1; ++i) {
            QPointF p1 = simplifiedPath.back() - path[i];
            QPointF p2 = path[i] - path[i + 1];
            if (std::abs(std::atan2(p1.y(), p1.x()) - std::atan2(p2.y(), p2.x())) > 0.01) {
                simplifiedPath.push_back(path[i]);
            }
        }
        simplifiedPath.push_back(path.back());
        return simplifiedPath;
    }

} // namespace Pathfinder