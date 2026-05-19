#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;

// Input uniform values
uniform vec2 screenCenter;
uniform vec2 mousePos;
uniform float zoom;

// Output fragment color
out vec4 finalColor;

void main()
{
    // Screen resolution (fixed for this demo, usually passed as uniform)
    vec2 resolution = vec2(800.0, 450.0);
    
    // Normalize coordinates (-1.0 to 1.0 range)
    vec2 z;
    z.x = (gl_FragCoord.x - resolution.x/2.0) / (0.5 * zoom * resolution.y);
    z.y = (gl_FragCoord.y - resolution.y/2.0) / (0.5 * zoom * resolution.y);

    // Constant 'c' based on mouse position
    vec2 c;
    c.x = (mousePos.x - resolution.x/2.0) / resolution.x * 2.0;
    c.y = (mousePos.y - resolution.y/2.0) / resolution.y * 2.0;

    int iterations = 0;
    int maxIterations = 255;

    while (iterations < maxIterations)
    {
        float x = (z.x * z.x - z.y * z.y) + c.x;
        float y = (2.0 * z.x * z.y) + c.y;

        if ((x * x + y * y) > 4.0) break;

        z.x = x;
        z.y = y;
        iterations++;
    }

    if (iterations < maxIterations)
    {
        // Simple color mapping
        float t = float(iterations) / float(maxIterations);
        finalColor = vec4(t * 0.5, t * 0.8, t, 1.0);
    }
    else
    {
        finalColor = vec4(0.0, 0.0, 0.0, 1.0);
    }
}
