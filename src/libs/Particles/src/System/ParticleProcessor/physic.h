#pragma once

//
// ================================================================
// Poisition - The position in which the particle is located
// Velocity - Current speed
// Forces - forces acting on a particle
// UMass - fabsf (masses)
// Drag - medium resistance (0 no resistance .. 1 full stop)
// TimeScale - time scaling ...
inline void SolvePhysic(Vector &Position, Vector &Velocity, const Vector &Forces, float UMass, float Drag,
                        float TimeScale)
{
    /*
      Drag = 1.0f - Drag;
      if (Drag < 0.0f) Drag = 0.0f;
      if (Drag > 1.0f) Drag = 1.0f;
    */
    auto Acceleration = Vector(0.0f);
    if (UMass)
        Acceleration = (Forces / UMass);

    Velocity += Acceleration * TimeScale;

    Position += ((Velocity * Drag) * TimeScale);
}

// Add the force of gravity to the current forces ...
inline void AddGravityForce(Vector &Forces, float Mass, float GravK)
{
    GravK *= 0.01f;
    if (GravK < 0)
        GravK = 0.0f;
    if (GravK > 1.0f)
        GravK = 1.0f;

    Forces += Vector(0.0f, -9.8f * Mass * GravK, 0.0f);
}
