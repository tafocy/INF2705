// Prénoms, noms et matricule des membres de l'équipe:
// - Cyrille TALLA (1930803)
// - Serge GNAVO (1869985)
//warning "Écrire les prénoms, noms et matricule des membres de l'équipe dans le fichier et commenter cette ligne"

#include <iostream>
#include <math.h>
#include "inf2705-matrice.h"
#include "inf2705-nuanceur.h"
#include "inf2705-fenetre.h"
#include "inf2705-forme.h"
#include "inf2705-theiere.h"
#include "Etat.h"
#include "Pipeline.h"
#include "Camera.h"
#include "Bonhomme.h"

// le bonhomme, le cube englobant
Bonhomme *bonhomme = NULL;
FormeCube *cubeFil = NULL;

void calculerPhysique( )
{
    bonhomme->calculerPhysique();
}

void chargerNuanceurs()
{
    // charger le nuanceur de base
    {
        // créer le programme
        progBase = glCreateProgram();

        // attacher le nuanceur de sommets
        {
            GLuint nuanceurObj = glCreateShader( GL_VERTEX_SHADER );
            glShaderSource( nuanceurObj, 1, &ProgNuanceur::chainesSommetsMinimal, NULL );
            glCompileShader( nuanceurObj );
            glAttachShader( progBase, nuanceurObj );
            ProgNuanceur::afficherLogCompile( nuanceurObj );
        }
        // attacher le nuanceur de fragments
        {
            GLuint nuanceurObj = glCreateShader( GL_FRAGMENT_SHADER );
            glShaderSource( nuanceurObj, 1, &ProgNuanceur::chainesFragmentsMinimal, NULL );
            glCompileShader( nuanceurObj );
            glAttachShader( progBase, nuanceurObj );
            ProgNuanceur::afficherLogCompile( nuanceurObj );
        }

        // faire l'édition des liens du programme
        glLinkProgram( progBase );
        ProgNuanceur::afficherLogLink( progBase );

        // demander la "Location" des variables
        if ( ( locmatrModel = glGetUniformLocation( progBase, "matrModel" ) ) == -1 ) std::cerr << "!!! pas trouvé la \"Location\" de matrModel" << std::endl;
        if ( ( locmatrVisu = glGetUniformLocation( progBase, "matrVisu" ) ) == -1 ) std::cerr << "!!! pas trouvé la \"Location\" de matrVisu" << std::endl;
        if ( ( locmatrProj = glGetUniformLocation( progBase, "matrProj" ) ) == -1 ) std::cerr << "!!! pas trouvé la \"Location\" de matrProj" << std::endl;
    }
}

void FenetreTP::initialiser()
{
    // donner la couleur de fond
    glClearColor( 0.15, 0.15, 0.15, 1.0 );

    // activer les états openGL
    glEnable( GL_DEPTH_TEST );

    // charger les nuanceurs
    chargerNuanceurs();
    glUseProgram( progBase );

    // créer le bonhomme
    bonhomme = new Bonhomme();

    // créer un cube englobant
    cubeFil = new FormeCube( 1.0, false );
}

void FenetreTP::conclure()
{
    delete bonhomme;
    delete cubeFil;
}

void afficherDecoration()
{
    // afficher la boîte englobante
    GLint locColor = glGetAttribLocation( progBase, "Color" );
    glVertexAttrib3f( locColor, 1.0, 0.5, 0.5 ); // équivalent au glColor() de OpenGL 2.x
    matrModel.PushMatrix();{
        matrModel.Translate( 0, 0, 0.5*Etat::dimBoite );
        matrModel.Scale( Etat::dimBoite, Etat::dimBoite, Etat::dimBoite );
        glUniformMatrix4fv( locmatrModel, 1, GL_FALSE, matrModel );
        cubeFil->afficher();
    }matrModel.PopMatrix(); glUniformMatrix4fv( locmatrModel, 1, GL_FALSE, matrModel );
}

void FenetreTP::afficherScene()
{
    // effacer l'écran et le tampon de profondeur
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glUseProgram( progBase );

    // définir le pipeline graphique
    matrProj.Perspective( 20.0, (GLdouble) largeur_ / (GLdouble) hauteur_, 0.1, 100.0 );
    glUniformMatrix4fv( locmatrProj, 1, GL_FALSE, matrProj ); // informer la carte graphique des changements faits à la matrice

    camera.definir();
    glUniformMatrix4fv( locmatrVisu, 1, GL_FALSE, matrVisu ); // informer la carte graphique des changements faits à la matrice

    matrModel.LoadIdentity();
    glUniformMatrix4fv( locmatrModel, 1, GL_FALSE, matrModel ); // informer la carte graphique des changements faits à la matrice

    // afficher les axes
    if ( Etat::afficheAxes ) FenetreTP::afficherAxes();

    // tracer la boite englobante
    afficherDecoration();

    // afficher des triangles en plein ou en fil de fer ?
    glPolygonMode( GL_FRONT_AND_BACK, Etat::modePolygone );
    // ne pas afficher les triangles dont on voit la face arrière ? (touche 'c')
    if ( Etat::culling ) glEnable( GL_CULL_FACE ); else glDisable( GL_CULL_FACE );

    // tracer le bonhomme
    bonhomme->afficher();

    // permuter tampons avant et arrière
    swap();
}

void FenetreTP::redimensionner( GLsizei w, GLsizei h )
{
    glViewport( 0, 0, w, h );
}

void FenetreTP::clavier( TP_touche touche )
{
    switch ( touche )
    {
    case TP_ECHAP:
    case TP_q: // Quitter l'application
        quit();
        break;

    case TP_x: // Activer/désactiver l'affichage des axes
        Etat::afficheAxes = !Etat::afficheAxes;
        std::cout << "// Affichage des axes ? " << ( Etat::afficheAxes ? "OUI" : "NON" ) << std::endl;
        break;

        // case TP_v: // Recharger les fichiers des nuanceurs et recréer le programme
        //    chargerNuanceurs();
        //    std::cout << "// Recharger nuanceurs" << std::endl;
        //    break;

    case TP_i: // Réinitiliaser le point de vue et le bonhomme
        camera.initVar(); bonhomme->initVar();
        break;
    case TP_g: // Permuter l'affichage en fil de fer ou plein
        Etat::modePolygone = ( Etat::modePolygone == GL_FILL ) ? GL_LINE : GL_FILL;
        break;
    case TP_c: // Permuter l'affichage des faces arrières
        Etat::culling = !Etat::culling;
        break;

    case TP_l: // Utiliser LookAt ou Translate+Rotate pour placer la caméra
        camera.modeLookAt = !camera.modeLookAt;
        std::cout << " camera.modeLookAt=" << camera.modeLookAt << std::endl;
        break;

    case TP_m: // Choisir le modèle affiché: cube, théière
        if ( ++Etat::modele > 2 ) Etat::modele = 1;
        std::cout << " Etat::modele=" << Etat::modele << std::endl;
        break;

    case TP_SOULIGNE:
    case TP_MOINS: // Reculer la caméra
        camera.dist += 0.1;
        break;
    case TP_PLUS: // Avancer la caméra
    case TP_EGAL:
        if ( camera.dist > 1.0 ) camera.dist -= 0.1;
        break;

    case TP_DROITE: // Déplacer le bonhomme vers +X
        if ( bonhomme->position.x + bonhomme->taille < 0.5*Etat::dimBoite ) bonhomme->position.x += 0.1;
        break;
    case TP_GAUCHE: // Déplacer le bonhomme vers -X
        if ( bonhomme->position.x - bonhomme->taille > -0.5*Etat::dimBoite ) bonhomme->position.x -= 0.1;
        break;
    case TP_PAGEPREC: // Déplacer le bonhomme vers +Y
        if ( bonhomme->position.y + bonhomme->taille < 0.5*Etat::dimBoite ) bonhomme->position.y += 0.1;
        break;
    case TP_PAGESUIV: // Déplacer le bonhomme vers -Y
        if ( bonhomme->position.y - bonhomme->taille > -0.5*Etat::dimBoite ) bonhomme->position.y -= 0.1;
        break;
    case TP_HAUT: // Déplacer le bonhomme vers +Z
        if ( bonhomme->position.z + bonhomme->taille < Etat::dimBoite ) bonhomme->position.z += 0.1;
        break;
    case TP_BAS: // Déplacer le bonhomme vers -Z
        if ( bonhomme->position.z - bonhomme->taille > 0.0 ) bonhomme->position.z -= 0.1;
        break;

    case TP_FIN:
    case TP_f: // Diminuer la taille du corps
        if ( bonhomme->taille > 0.25 ) bonhomme->taille -= 0.05;
        bonhomme->verifierAngles();
        std::cout << " bonhomme->taille=" << bonhomme->taille << " bonhomme->angleCorps=" << bonhomme->angleCorps << " bonhomme->angleAile=" << bonhomme->angleAile << " bonhomme->angleJambe=" << bonhomme->angleJambe << std::endl;
        break;
    case TP_DEBUT:
    case TP_r: // Augmenter la taille du corps
        bonhomme->taille += 0.05;
        bonhomme->verifierAngles();
        std::cout << " bonhomme->taille=" << bonhomme->taille << " bonhomme->angleCorps=" << bonhomme->angleCorps << " bonhomme->angleAile=" << bonhomme->angleAile << " bonhomme->angleJambe=" << bonhomme->angleJambe << std::endl;
        break;

    case TP_VIRGULE: // Diminuer l'angle de rotation du bonhomme
        bonhomme->angleCorps -= 2.0;
        std::cout << " bonhomme->taille=" << bonhomme->taille << " bonhomme->angleCorps=" << bonhomme->angleCorps << " bonhomme->angleAile=" << bonhomme->angleAile << " bonhomme->angleJambe=" << bonhomme->angleJambe << std::endl;
        break;
    case TP_POINT: // Augmenter l'angle de rotation du bonhomme
        bonhomme->angleCorps += 2.0;
        std::cout << " bonhomme->taille=" << bonhomme->taille << " bonhomme->angleCorps=" << bonhomme->angleCorps << " bonhomme->angleAile=" << bonhomme->angleAile << " bonhomme->angleJambe=" << bonhomme->angleJambe << std::endl;
        break;

    case TP_CROCHETDROIT:
    case TP_o: // Diminuer l'angle des jambes
        bonhomme->angleJambe -= 2.0;
        bonhomme->verifierAngles();
        std::cout << " bonhomme->taille=" << bonhomme->taille << " bonhomme->angleCorps=" << bonhomme->angleCorps << " bonhomme->angleAile=" << bonhomme->angleAile << " bonhomme->angleJambe=" << bonhomme->angleJambe << std::endl;
        break;
    case TP_CROCHETGAUCHE:
    case TP_p: // Augmenter l'angle des jambes
        bonhomme->angleJambe += 2.0;
        bonhomme->verifierAngles();
        std::cout << " bonhomme->taille=" << bonhomme->taille << " bonhomme->angleCorps=" << bonhomme->angleCorps << " bonhomme->angleAile=" << bonhomme->angleAile << " bonhomme->angleJambe=" << bonhomme->angleJambe << std::endl;
        break;

    case TP_j: // Diminuer l'angle des ailes
        bonhomme->angleAile -= 2.0;
        bonhomme->verifierAngles();
        std::cout << " bonhomme->taille=" << bonhomme->taille << " bonhomme->angleCorps=" << bonhomme->angleCorps << " bonhomme->angleAile=" << bonhomme->angleAile << " bonhomme->angleJambe=" << bonhomme->angleJambe << std::endl;
        break;
    case TP_u: // Augmenter l'angle des ailes
        bonhomme->angleAile += 2.0;
        bonhomme->verifierAngles();
        std::cout << " bonhomme->taille=" << bonhomme->taille << " bonhomme->angleCorps=" << bonhomme->angleCorps << " bonhomme->angleAile=" << bonhomme->angleAile << " bonhomme->angleJambe=" << bonhomme->angleJambe << std::endl;
        break;

    case TP_b: // Incrémenter la dimension de la boite
        Etat::dimBoite += 0.05;
        std::cout << " Etat::dimBoite=" << Etat::dimBoite << std::endl;
        break;
    case TP_h: // Décrémenter la dimension de la boite
        Etat::dimBoite -= 0.05;
        if ( Etat::dimBoite < 1.0 ) Etat::dimBoite = 1.0;
        std::cout << " Etat::dimBoite=" << Etat::dimBoite << std::endl;
        break;

    case TP_ESPACE: // Mettre en pause ou reprendre l'animation
        Etat::enmouvement = !Etat::enmouvement;
        break;

    default:
        std::cout << " touche inconnue : " << (char) touche << std::endl;
        imprimerTouches();
        break;
    }
}

glm::ivec2 sourisPosPrec(0,0);
static bool pressed = false;
void FenetreTP::sourisClic( int button, int state, int x, int y )
{
    // button est un parmi { TP_BOUTON_GAUCHE, TP_BOUTON_MILIEU, TP_BOUTON_DROIT }
    // state  est un parmi { TP_PRESSE, DL_RELEASED }
    pressed = ( state == TP_PRESSE );
    switch ( button )
    {
    case TP_BOUTON_GAUCHE: // Déplacer (modifier angles) la caméra
        sourisPosPrec.x = x;
        sourisPosPrec.y = y;
        break;
    }
}

void FenetreTP::sourisMolette( int x, int y )
{
    const int sens = +1;
    camera.dist -= 0.2 * sens*y;
    if ( camera.dist < 15.0 ) camera.dist = 15.0;
    else if ( camera.dist > 70.0 ) camera.dist = 70.0;
}

void FenetreTP::sourisMouvement( int x, int y )
{
    if ( pressed )
    {
        int dx = x - sourisPosPrec.x;
        int dy = y - sourisPosPrec.y;
        camera.theta -= dx / 3.0;
        camera.phi   -= dy / 3.0;
        sourisPosPrec.x = x;
        sourisPosPrec.y = y;
        camera.verifierAngles();
    }
}

int main( int argc, char *argv[] )
{
    // créer une fenêtre
    FenetreTP fenetre( "INF2705 TP" );

    // allouer des ressources et définir le contexte OpenGL
    fenetre.initialiser();

    bool boucler = true;
    while ( boucler )
    {
        // mettre à jour la physique
        calculerPhysique( );

        // affichage
        fenetre.afficherScene();

        // récupérer les événements et appeler la fonction de rappel
        boucler = fenetre.gererEvenement();
    }

    // détruire les ressources OpenGL allouées
    fenetre.conclure();

    return 0;
}
