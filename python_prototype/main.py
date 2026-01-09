import pygame
import math

# --- Configuration ---
WIDTH, HEIGHT = 800, 600
GRAVITY = 0.5
BOUNCE_DAMPING = 0.7
FRICTION = 0.999
ITERATIONS = 5  # Higher = Stiffer constraints

class Point:
    def __init__(self, x, y):
        self.x = x
        self.y = y
        self.old_x = x
        self.old_y = y
        self.pinned = False

    def update(self):
        if self.pinned:
            return

        # Verlet Integration
        # velocity is inferred from the difference between current and old position
        vx = (self.x - self.old_x) * FRICTION
        vy = (self.y - self.old_y) * FRICTION

        # Save current state
        self.old_x = self.x
        self.old_y = self.y

        # Apply new position
        self.x += vx
        self.y += vy + GRAVITY

        # Floor Collision
        if self.y > HEIGHT:
            self.y = HEIGHT
            self.old_y = self.y + vy * BOUNCE_DAMPING
        
        # Wall Collision
        if self.x > WIDTH:
            self.x = WIDTH
            self.old_x = self.x + vx * BOUNCE_DAMPING
        elif self.x < 0:
            self.x = 0
            self.old_x = self.x + vx * BOUNCE_DAMPING

class Stick:
    def __init__(self, p1, p2, length=None):
        self.p1 = p1
        self.p2 = p2
        if length is None:
            dx = self.p1.x - self.p2.x
            dy = self.p1.y - self.p2.y
            self.length = math.sqrt(dx * dx + dy * dy)
        else:
            self.length = length

    def update(self):
        dx = self.p1.x - self.p2.x
        dy = self.p1.y - self.p2.y
        dist = math.sqrt(dx * dx + dy * dy)

        if dist == 0:
            return

        diff = self.length - dist
        percent = diff / dist / 2.0

        offset_x = dx * percent
        offset_y = dy * percent

        if not self.p1.pinned:
            self.p1.x += offset_x
            self.p1.y += offset_y
        if not self.p2.pinned:
            self.p2.x -= offset_x
            self.p2.y -= offset_y

    def draw(self, screen):
        pygame.draw.line(screen, (255, 255, 255), (self.p1.x, self.p1.y), (self.p2.x, self.p2.y), 2)

class Solver:
    def __init__(self):
        self.points = []
        self.sticks = []

    def update(self):
        # 1. Apply Forces
        for p in self.points:
            p.update()

        # 2. Solve Constraints (Stiffness)
        for _ in range(ITERATIONS):
            for s in self.sticks:
                s.update()

    def draw(self, screen):
        for s in self.sticks:
            s.draw(screen)
        for p in self.points:
            pygame.draw.circle(screen, (255, 100, 100), (int(p.x), int(p.y)), 4)

# --- Main Loop ---
pygame.init()
screen = pygame.display.set_mode((WIDTH, HEIGHT))
pygame.display.set_caption("Soft Body Physics - Prototype")
clock = pygame.time.Clock()

solver = Solver()

# Create a Box (4 points)
p1 = Point(300, 100)
p2 = Point(400, 100)
p3 = Point(400, 200)
p4 = Point(300, 200)

solver.points.extend([p1, p2, p3, p4])

# Edges
solver.sticks.append(Stick(p1, p2))
solver.sticks.append(Stick(p2, p3))
solver.sticks.append(Stick(p3, p4))
solver.sticks.append(Stick(p4, p1))

# Cross-braces (Internal Support)
solver.sticks.append(Stick(p1, p3))
# solver.sticks.append(Stick(p2, p4)) # <-- Uncomment this to make the box rigid!

running = True
while running:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False
        # Reset on Click
        if event.type == pygame.MOUSEBUTTONDOWN:
            p1.x, p1.y = 300, 100; p1.old_x, p1.old_y = 300, 100
            p2.x, p2.y = 400, 100; p2.old_x, p2.old_y = 400, 100
            p3.x, p3.y = 400, 200; p3.old_x, p3.old_y = 400, 200
            p4.x, p4.y = 300, 200; p4.old_x, p4.old_y = 300, 200

    solver.update()

    screen.fill((30, 30, 30))
    solver.draw(screen)
    pygame.display.flip()
    clock.tick(60)

pygame.quit()