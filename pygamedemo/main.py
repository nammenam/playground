import pygame
from sys import exit


class Ring(pygame.sprite.Sprite):
    def __init__(self, pos):
        super().__init__()
        # Create a fixed-size surface for the max radius (100 radius = 200x200 surface)
        self.image = pygame.Surface((200, 200), pygame.SRCALPHA)
        self.rect = self.image.get_rect(center=pos)

        self.radius = 5
        self.alpha = 125
        self.center_pos = pos

    def update(self):
        self.radius += 1
        self.alpha -= 1.5

        # Remove the sprite if it gets too big or invisible
        if self.radius >= 100 or self.alpha <= 0:
            self.kill()
        else:
            # Clear the surface completely, then draw the new, larger circle
            self.image.fill((0, 0, 0, 0))
            color = (255, 255, 255, int(max(0, self.alpha)))
            pygame.draw.circle(self.image, color, (100, 100), int(self.radius), 5)


class And(pygame.sprite.Sprite):
    def __init__(self, pos):
        super().__init__()

        # Try to load the image, fallback to a colored square if it fails
        try:
            self.original_image = pygame.image.load(
                "textures/topviewduck.png"
            ).convert_alpha()
            self.original_image = pygame.transform.scale(self.original_image, (40, 40))
            self.original_image = pygame.transform.rotate(self.original_image, -90)
        except FileNotFoundError:
            self.original_image = pygame.Surface((40, 40), pygame.SRCALPHA)
            self.original_image.fill("Yellow")

        self.image = self.original_image
        self.rect = self.image.get_rect(center=pos)

        # Vector properties
        self.pos = pygame.math.Vector2(pos)
        self.retning = pygame.math.Vector2(0, -1)  # Default pointing up
        self.vinkel = 90
        self.aks = 0

    def update(self):
        keys = pygame.key.get_pressed()

        # Rotation
        if keys[pygame.K_a]:
            self.vinkel += 2.5
        if keys[pygame.K_d]:
            self.vinkel -= 2.5

        # Update direction vector based on angle
        self.retning = pygame.math.Vector2(1, 0).rotate(-self.vinkel)

        # Acceleration
        if keys[pygame.K_w]:
            self.aks = min(self.aks + 0.1, 4)  # Max speed is 4
        else:
            self.aks = max(self.aks - 0.05, 0)  # Friction/Deceleration

        # Movement
        if self.aks > 0:
            self.pos += self.retning * self.aks
            self.rect.center = round(self.pos.x), round(self.pos.y)

        # Rotate the image and recenter the rect to prevent wobbling
        self.image = pygame.transform.rotate(self.original_image, self.vinkel - 90)
        self.rect = self.image.get_rect(center=self.rect.center)


def main():
    pygame.init()
    pygame.display.set_caption("plopp:)")
    screen = pygame.display.set_mode((1200, 800))
    clock = pygame.time.Clock()

    try:
        bakgrunn = pygame.image.load("textures/bakgrunn.jpg").convert()
    except FileNotFoundError:
        bakgrunn = pygame.Surface((1200, 800))
        bakgrunn.fill("darkblue")

    # Sprite Groups
    all_sprites = pygame.sprite.Group()
    ringer_group = pygame.sprite.Group()

    # Create the duck
    spiller = And((600, 400))
    all_sprites.add(spiller)

    # Timers
    idleringer_timer = pygame.USEREVENT + 1
    pygame.time.set_timer(idleringer_timer, 600)

    and_move_timer = pygame.USEREVENT + 2
    pygame.time.set_timer(and_move_timer, 100)

    mouse_ring_counter = 0

    while True:
        mouse = pygame.mouse.get_pos()
        keys = pygame.key.get_pressed()

        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                pygame.quit()
                exit()

            # Automatic ripples from the duck
            if event.type == idleringer_timer and not keys[pygame.K_w]:
                ny_ring = Ring(spiller.rect.center)
                ringer_group.add(ny_ring)
                all_sprites.add(ny_ring)

            if event.type == and_move_timer and keys[pygame.K_w]:
                ny_ring = Ring(spiller.rect.center)
                ringer_group.add(ny_ring)
                all_sprites.add(ny_ring)

            # Manual ripples from the mouse
            if event.type == pygame.MOUSEBUTTONDOWN and event.button == 1:
                ny_ring = Ring(mouse)
                ringer_group.add(ny_ring)
                all_sprites.add(ny_ring)
                mouse_ring_counter = 1

            if event.type == pygame.MOUSEMOTION and pygame.mouse.get_pressed()[0]:
                mouse_ring_counter += 1
                if mouse_ring_counter % 3 == 0:
                    ny_ring = Ring(mouse)
                    ringer_group.add(ny_ring)
                    all_sprites.add(ny_ring)

        # Update all logic
        all_sprites.update()

        # Draw everything
        screen.blit(bakgrunn, (0, 0))

        # Draw rings first so they appear under the duck
        ringer_group.draw(screen)
        screen.blit(spiller.image, spiller.rect)

        pygame.display.update()
        clock.tick(60)


if __name__ == "__main__":
    main()
