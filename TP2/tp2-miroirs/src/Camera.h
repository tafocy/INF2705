#ifndef __CAMERA_H__
#define __CAMERA_H__

//
// variables pour définir le point de vue
//
class Camera
{
public:
    Camera() { theta = -90.0; phi = 75.0; dist = 35.0; }
    void definir()
    {
        matrVisu.LookAt( dist*cos(glm::radians(theta))*sin(glm::radians(phi)),
                         dist*sin(glm::radians(theta))*sin(glm::radians(phi)),
                         dist*cos(glm::radians(phi)),
                         0, 0, 0,
                         0, 0, 1 );
    }
    void verifierAngles() // vérifier que les angles ne débordent pas les valeurs permises
    {
        const GLdouble MINPHI = 0.01, MAXPHI = 180.0 - 0.01;
        phi = glm::clamp( phi, MINPHI, MAXPHI );
    }
    double theta;         // angle de rotation de la caméra (coord. sphériques)
    double phi;           // angle de rotation de la caméra (coord. sphériques)
    double dist;          // distance (coord. sphériques)
} camera;

#endif
