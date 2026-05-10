# Feature: Lighting

> Point light inside the golf ball, projecting onto the floor using the Phong model.

---

## Light Type

**Point light** — emits in all directions from a single position.

Initially positioned at the **centre of the golf ball** (world origin, before any scene rotation).

---

## Phong Point Light Model (Chapter 4 of textbook)

The lighting computation happens **per vertex** on the floor (vertex shader or fragment shader —
fragment shader preferred for smoother results).

### Components

```
ambient  = ambientStrength * lightColour
diffuse  = max(dot(normal, lightDir), 0.0) * lightColour
specular = pow(max(dot(reflectDir, viewDir), 0.0), shininess) * lightColour

attenuation = 1.0 / (constant + linear * dist + quadratic * dist^2)

lighting = (ambient + (diffuse + specular) * attenuation)
```

Where:
- `lightDir` = normalise(lightPos - fragmentWorldPos)
- `dist` = length(lightPos - fragmentWorldPos)
- `reflectDir` = reflect(-lightDir, normal)
- `viewDir` = normalise(cameraPos - fragmentWorldPos)
- Attenuation constants: choose values that make the light falloff visible but not too harsh.
  Suggested starting values: constant=1.0, linear=0.09, quadratic=0.032.

### Floor Final Colour Formula

The floor fragment's final colour combines three sources:

```
floorFinal = floorBaseColour * (ambient + diffuse * attenuation) * lightColour * glassColour
```

More precisely:
- The floor colour is first set by the floor's own base colour.
- The light contributes intensity (attenuated by distance) and its colour tint.
- The glass colour is applied as a multiplicative tint on top of the light (the light passes through the glass).
- Areas far from the light receive mostly ambient + floor base colour.

### Glass Colour Contribution

The glass acts as a coloured filter. The light colour that reaches the floor is:
```
effectiveLightColour = lightColour * glassColour
```
This is computed on the CPU and passed as a single `vec3` uniform to the fragment shader,
so the shader doesn't need to know about the glass separately.

---

## Light vs Golf Ball

- The light's projection **only affects the floor** (spec requirement).
- The golf ball itself is not lit by the internal light in the base requirement.
- (Bonus: allow light to affect the ball for +2 marks.)

### Implementation

In the fragment shader, use a `uniform bool isFloor` flag (or draw floor and sphere with separate
shader programs / draw calls) so that the lighting computation is only applied to floor fragments.

The simplest approach: two separate draw calls, each with a shared shader that has a `uniform bool applyLighting` flag.

---

## Light Translation

The light starts at sphere centre `(0, 0, 0)` in local/scene space.

Arrow keys and `<`/`>` translate the light within the scene's **local coordinate system**,
meaning if the scene has been rotated, the light still moves along the rotated axes.

To achieve this:
- Store `lightLocalPos` as a `vec3` in scene-local space.
- When computing the light's world-space position for the shader, apply the scene's current
  rotation matrix: `lightWorldPos = sceneRotMatrix * lightLocalPos`.
- Pass `lightWorldPos` as a uniform to the fragment shader.

This ensures that pressing "up arrow" always moves the light upward relative to the current
view of the scene, not the global Y axis.

---

## Uniforms Passed to Shader

```
uniform vec3 lightPos;          // world-space position (after scene rotation applied)
uniform vec3 lightColour;       // current light colour (RGB)
uniform vec3 effectiveLightColour; // lightColour * glassColour
uniform float lightConstant;
uniform float lightLinear;
uniform float lightQuadratic;
uniform vec3 cameraPos;         // for specular calculation
uniform bool applyLighting;     // true for floor, false for sphere
```

---

## Key Decisions to Log in `progress.md`

- [ ] Whether lighting is computed in vertex shader or fragment shader (recommend fragment).
- [ ] Attenuation constants chosen.
- [ ] Whether ambient is included or just diffuse + specular.
- [ ] Whether specular is included (adds realism, not strictly required by spec).
- [ ] Whether one shader handles both floor and sphere with a flag, or two separate shaders.
