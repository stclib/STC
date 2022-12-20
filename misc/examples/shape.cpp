// Demo of polymorphism in C++

#include <iostream>
#include <memory>
#include <vector>

// Shape definition
// ============================================================

struct Point {
    float x, y;
};

std::ostream& operator<<(std::ostream& os, const Point& p) {
    os << " (" << p.x << "," << p.y << ")";
    return os;
}

struct Shape {
    virtual ~Shape();
    virtual void draw() const = 0;

    uint32_t color;
    uint16_t style;
    uint8_t thickness;
    uint8_t hardness;
};

Shape::~Shape()
{
    std::cout << "base destructed" << std::endl;
}

// Triangle implementation
// ============================================================

struct Triangle : public Shape
{
    Triangle(Point a, Point b, Point c);
    void draw() const override;

    private: Point p[3];
};


Triangle::Triangle(Point a, Point b, Point c)
    : p{a, b, c} {}

void Triangle::draw() const
{
    std::cout << "Triangle :" 
              << p[0] << p[1] << p[2]
              << std::endl;
}


// Polygon implementation
// ============================================================


struct Polygon : public Shape
{
    ~Polygon();
    void draw() const override;
    void addPoint(const Point& p);

    private: std::vector<Point> points;
};


void Polygon::addPoint(const Point& p)
{
    points.push_back(p);
}

Polygon::~Polygon()
{
    std::cout << "poly destructed" << std::endl;
}

void Polygon::draw() const
{
    std::cout << "Polygon  :";
    for (auto& p : points)
        std::cout << p ;
    std::cout << std::endl;
}


// Test
// ============================================================

void testShape(const Shape* shape)
{
    shape->draw();
}

#include <array>

int main(void)
{
    std::vector<std::unique_ptr<Shape>> shapes;

    auto tri1 = std::make_unique<Triangle>(Point{5, 7}, Point{12, 7}, Point{12, 20});
    auto pol1 = std::make_unique<Polygon>();
    auto pol2 = std::make_unique<Polygon>();

    for (auto& p: std::array<Point, 4>
            {{{50, 72}, {123, 73}, {127, 201}, {828, 333}}})
        pol1->addPoint(p);

    for (auto& p: std::array<Point, 5>
            {{{5, 7}, {12, 7}, {12, 20}, {82, 33}, {17, 56}}})
        pol2->addPoint(p);

    shapes.push_back(std::move(tri1));
    shapes.push_back(std::move(pol1));
    shapes.push_back(std::move(pol2));

    for (auto& shape: shapes)
        testShape(shape.get());
}
