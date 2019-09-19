#include "Etat.h"

SINGLETON_DECLARATION_CPP(Etat);

bool       Etat::enSelection = false;
bool       Etat::enmouvement = true;
bool       Etat::afficheAxes = true;
bool       Etat::attenuation = false;
glm::vec4  Etat::bDim = glm::vec4( 10.0, 10.0, 10.0, 1.0 );
glm::ivec2 Etat::sourisPosPrec = glm::ivec2(0);
glm::vec4  Etat::planRayonsX = glm::vec4( 1, 0, 0, -4.0 );
float      Etat::dt = 1.0/60.0;
