#ifndef __BONHOMME_H__
#define __BONHOMME_H__

// les formes
FormeCube *cube = NULL;
FormeQuad *quad = NULL;
FormeSphere *sphere = NULL;
FormeCylindre *cylindre = NULL;
// (partie 1) Vous devez vous servir des quatre fonctions ci-dessous (*sans les modifier*) pour tracer tous les parties des objets.
// affiche un cylindre de rayon 1.0 et de longueur 1.0, dont la base est centrée en (0,0,0)
void afficherCylindre( ) { cylindre->afficher(); }
// affiche une sphère de rayon 1.0, centrée en (0,0,0)
void afficherSphere( ) { sphere->afficher(); }
// affiche un cube d'arête 1.0, centrée en (0,0,0)
void afficherCube( ) { cube->afficher(); }
// affiche un aile d'arête 1
void afficherQuad( ) { quad->afficher(); }

// affiche la position courante du repère (pour débogage)
void afficherRepereCourant( int num = 0 )
{
    glUniformMatrix4fv( locmatrModel, 1, GL_FALSE, matrModel );
    FenetreTP::afficherAxes( 1.5, 3.0 );
}

// partie 1: le bonhomme
class Bonhomme
{
public:
    Bonhomme()
    {
        initVar();
        // créer le bonhomme graphique
        initialiserGraphique();

        // créer quelques autres formes
        cube = new FormeCube( 1.0, true );
        quad = new FormeQuad( 1.0, true );
        sphere = new FormeSphere( 1.0, 8, 8, true );
        cylindre = new FormeCylindre( 1.0, 1.0, 1.0, 16, 1, true );
    }
    ~Bonhomme()
    {
        conclureGraphique();
        delete cube;
        delete quad;
        delete sphere;
        delete cylindre;
    }

    void initVar() { position = glm::vec3(0.0, 0.0, 2.0); taille = 0.5; angleCorps = angleAile = angleJambe = 0.0; }
    void verifierAngles() // vérifier que les angles ne débordent pas les valeurs permises
    {
        if ( angleJambe > 90.0 ) angleJambe = 90.0; else if ( angleJambe < 0.0 ) angleJambe = 0.0;
        if ( angleAile > 90.0 ) angleAile = 90.0; else if ( angleAile < 0.0 ) angleAile = 0.0;
    }

    void initialiserGraphique()
    {
        GLint prog = 0; glGetIntegerv( GL_CURRENT_PROGRAM, &prog );
        if ( prog <= 0 )
        {
            std::cerr << "Pas de programme actif!" << std::endl;
            locVertex = locColor = -1;
            return;
        }
        if ( ( locVertex = glGetAttribLocation( prog, "Vertex" ) ) == -1 ) std::cerr << "!!! pas trouvé la \"Location\" de Vertex" << std::endl;
        if ( ( locColor = glGetAttribLocation( prog, "Color" ) ) == -1 ) std::cerr << "!!! pas trouvé la \"Location\" de Color" << std::endl;

        // allouer les objets OpenGL
        glGenVertexArrays( 1, &vao );

        // initialiser le VAO pour la théière
        glBindVertexArray( vao );

        // (partie 2) MODIFICATIONS ICI ...
        // créer le VBO pour les sommets

        // créer le VBO la connectivité

        glBindVertexArray(0);
    }

    void conclureGraphique()
    {
        glDeleteBuffers( 1, &vboTheiereSommets );
        glDeleteBuffers( 1, &vboTheiereConnec );
    }

    // (partie 2) Vous modifierez cette fonction pour utiliser les VBOs
    // affiche une théière, dont la base est centrée en (0,0,0)
    void afficherTheiere()
    {
        glBindVertexArray( vao );
        // (partie 2) MODIFICATIONS ICI ...
        // vous pouvez utiliser temporairement cette fonction pour la première partie du TP, mais vous ferez mieux dans la seconde partie du TP
        glBegin( GL_TRIANGLES );
        for ( unsigned int i = 0 ; i < sizeof(gTheiereConnec)/sizeof(GLuint) ; i++ )
            glVertex3fv( &(gTheiereSommets[3*gTheiereConnec[i]] ) );
        glEnd( );
        glBindVertexArray(0);
    }

    // afficher le corps du bonhomme
    void afficherCorps()
    {
        // donner la couleur du corps
        glVertexAttrib3f( locColor, 0.5, 0.5, 1.0 ); // bleu foncé; équivalent au glColor() de OpenGL 2.x

        // montrer le repère à la position courante
        //afficherRepereCourant( );
		
        // afficher le corps
        matrModel.PushMatrix();{
            // ajouter une ou des transformations pour bien positionner le corps du bonhomme de taille "taille"
				matrModel.Translate( 0, 0, 0 );
				matrModel.Scale(taille, taille, 2.0*taille);
				
				//afficherRepereCourant( );

            // ==> Avant de tracer, on doit informer la carte graphique des changements faits à la matrice de modélisation
            glUniformMatrix4fv( locmatrModel, 1, GL_FALSE, matrModel );
            afficherCylindre();

        }matrModel.PopMatrix(); glUniformMatrix4fv( locmatrModel, 1, GL_FALSE, matrModel );
    }

    // afficher la tête du bonhomme
    void afficherTete()
    {
        // donner la couleur de la tête
        glVertexAttrib3f( locColor, 1.0, 1.0, 0.0 ); // jaune; équivalent au glColor() de OpenGL 2.x

        // montrer le repère à la position courante
    //    afficherRepereCourant( );

        matrModel.PushMatrix();{

            // afficher le bon modèle
            switch ( Etat::modele )
            {
            default:
            case 1: // une sphère
           
                matrModel.Translate( 0.0, 0.0,3*taille); // (bidon) À MODIFIER
                matrModel.Scale(taille, taille, taille);
                glUniformMatrix4fv( locmatrModel, 1, GL_FALSE, matrModel );
                afficherSphere();
                break;

            case 2: // la théière
                matrModel.Scale( 0.45, 0.45, 0.45 );
                glUniformMatrix4fv( locmatrModel, 1, GL_FALSE, matrModel );
                afficherTheiere();
                break;

            }
        }matrModel.PopMatrix(); glUniformMatrix4fv( locmatrModel, 1, GL_FALSE, matrModel );
    }

    // afficher les deux ailes
    void afficherAiles()
    {
        // donner la couleur des ailes
        glVertexAttrib3f( locColor, 1.0, 0.5, 1.0 ); // magenta clair; équivalent au glColor() de OpenGL 2.x

        // ajouter une ou des transformations afin de tracer des *ailes carrées*, de la même largeur que le corps
            
            //les ailes 
            matrModel.PushMatrix();{
         
         //  afficherRepereCourant( );
         
           matrModel.Translate(0.0,-taille,2*taille);  
          
           matrModel.Rotate(-angleAile,1.0,0.0,0.0); 
           matrModel.Translate(0.0,-taille,0.0);
           matrModel.Scale(2*taille,2*taille,1.0);
           
           // matrModel.Translate(0.0,0.0,5+2*taille);
            
            // afficherRepereCourant( ); // débogage
            glUniformMatrix4fv( locmatrModel, 1, GL_FALSE, matrModel );
            afficherQuad();
        }matrModel.PopMatrix(); glUniformMatrix4fv( locmatrModel, 1, GL_FALSE, matrModel );
        // ajouter une ou des transformations afin de tracer des *ailes carrées*, de la même largeur que le corps
             
            matrModel.PushMatrix();{
            matrModel.Translate( 0.0, taille, 2*taille ); 
       
             matrModel.Rotate(angleAile,1.0,0.0,0.0); 
            matrModel.Translate(0.0,taille,0.0);
            matrModel.Scale(2*taille,2*taille,1.0);
            
            // afficherRepereCourant( ); // débogage
            glUniformMatrix4fv( locmatrModel, 1, GL_FALSE, matrModel );
            afficherQuad();
        }matrModel.PopMatrix(); glUniformMatrix4fv( locmatrModel, 1, GL_FALSE, matrModel );
        
        
        //creer la deuxieme ailes
    }

    // afficher les deux bras et les deux jambes
    void afficherBrasJambes()
    {
        glVertexAttrib3f( locColor, 0.0, 1.0, 0.0 ); // vert; équivalent au glColor() de OpenGL 2.x

        // ajouter une ou des transformations afin de tracer les bras et les jambes de largeur "largMembre" et longueur "longMembre"
        // les bras
        
       matrModel.PushMatrix();{
            //matrModel.Translate( 0.0, 0.0,0.0); // (bidon) À MODIFIER
            matrModel.Translate(0.0, taille+(longMembre/2)*sin(45),2*taille-longMembre/2 );
            matrModel.Rotate(45,1.0,0.0,0.0);
            matrModel.Scale(largMembre,largMembre,longMembre);//mise a l'echelle de la premiere jambe
            
            // afficherRepereCourant( ); // débogage
            glUniformMatrix4fv( locmatrModel, 1, GL_FALSE, matrModel );
            afficherCube();
        }matrModel.PopMatrix(); glUniformMatrix4fv( locmatrModel, 1, GL_FALSE, matrModel );
       
       
       
        matrModel.PushMatrix();{
            //matrModel.Translate( 0.0, 0.0,0.0); // (bidon) À MODIFIER
            matrModel.Translate(0.0, -(taille+(longMembre/2)*sin(45)),-longMembre/2 +2*taille);
            matrModel.Rotate(-45,1.0,0.0,0.0);
            matrModel.Scale(largMembre,largMembre,longMembre);//mise a l'echelle de la premiere jambe
            
            // afficherRepereCourant( ); // débogage
            glUniformMatrix4fv( locmatrModel, 1, GL_FALSE, matrModel );
            afficherCube();
        }matrModel.PopMatrix(); glUniformMatrix4fv( locmatrModel, 1, GL_FALSE, matrModel );
       
        // les jambes
        // ...
         matrModel.PushMatrix();{
            matrModel.Translate( 0.0, -taille,0 ); // (bidon) À MODIFIER
            
            matrModel.Rotate(angleJambe,0.0,1.0,0.0);
            matrModel.Translate(0.0,0.0,-longMembre/2);
            matrModel.Scale(largMembre,largMembre,longMembre);//mise a l'echelle de la premiere jambe
            //matrModel.Scale(0.5,0.25,1.0);
            // afficherRepereCourant( ); // débogage
            glUniformMatrix4fv( locmatrModel, 1, GL_FALSE, matrModel );
            afficherCube();
        }matrModel.PopMatrix(); glUniformMatrix4fv( locmatrModel, 1, GL_FALSE, matrModel );
        
        
        
 
        matrModel.PushMatrix();{
            matrModel.Translate( 0.0, taille,0 ); // (bidon) À MODIFIER
            
            matrModel.Rotate(angleJambe,0.0,1.0,0.0);
            matrModel.Translate(0.0,0.0,-longMembre/2);
            matrModel.Scale(largMembre,largMembre,longMembre);//mise a l'echelle de la deuxiemejambe
            
            // afficherRepereCourant( ); // débogage
            glUniformMatrix4fv( locmatrModel, 1, GL_FALSE, matrModel );
            afficherCube();
        }matrModel.PopMatrix(); glUniformMatrix4fv( locmatrModel, 1, GL_FALSE, matrModel );
        
        
        
         //matrModel.PushMatrix();{
          //  matrModel.Translate( 0.0, taille,5.0 -longMembre/2 ); // (bidon) À MODIFIER
           // matrModel.Scale(0.5,0.25,1.0);//mise a l'echelle de la premiere jambe
             // matrModel.Rotate(-angleJambe,0.0,1.0,0.0);
             // matrModel.Scale(largMembre,largMembre,longMembre);
            // afficherRepereCourant( ); // débogage
           // glUniformMatrix4fv( locmatrModel, 1, GL_FALSE, matrModel );
          //  afficherCube();
       // }matrModel.PopMatrix(); glUniformMatrix4fv( locmatrModel, 1, GL_FALSE, matrModel );
        
       
    }

    void afficher()
    {
        // afficherRepereCourant( ); // débogage
        matrModel.PushMatrix();{ // sauvegarder la tranformation courante

            // ajouter une ou des transformations afin de centrer le haut du corps à la position courante "position[]" et de tourner son corps de l'angle "angleCorps"
			matrModel.Translate( position.x , position.y , position.z  );
			matrModel.Rotate(angleCorps,0.0,0.0,1.0);
            // afficher le corps
            afficherCorps();

            // afficher la tête
            afficherTete();

            // afficher les deux ailes
            afficherAiles();

            // afficher les deux jambes
            afficherBrasJambes();

        }matrModel.PopMatrix(); glUniformMatrix4fv( locmatrModel, 1, GL_FALSE, matrModel );
        glUniformMatrix4fv( locmatrModel, 1, GL_FALSE, matrModel ); // informer ...
    }

    void calculerPhysique()
    {
        if ( Etat::enmouvement )
        {
            static int sens[6] = { +1, +1, +1, +1, +1, +1 };
            glm::vec3 vitesse( 0.03, 0.02, 0.05 );
            // mouvement en X
            if ( position.x-taille <= -0.5*Etat::dimBoite ) sens[0] = +1.0;
            else if ( position.x+taille >= 0.5*Etat::dimBoite ) sens[0] = -1.0;
            position.x += vitesse.x * sens[0];
            // mouvement en Y
            if ( position.y-taille <= -0.5*Etat::dimBoite ) sens[1] = +1.0;
            else if ( position.y+taille >= 0.5*Etat::dimBoite ) sens[1] = -1.0;
            position.y += vitesse.y * sens[1];
            // mouvement en Z
            if ( position.z-taille <= 0.0 ) sens[2] = +1.0;
            else if ( position.z+taille >= Etat::dimBoite ) sens[2] = -1.0;
            position.z += vitesse.z * sens[2];

            // angle des jambes et des ailes
            if ( angleJambe <= 0.0 ) sens[3] = +1.0;
            else if ( angleJambe >= 90.0 ) sens[3] = -1.0;
            angleJambe += 1.0 * sens[3];
            if ( angleAile <= 0.0 ) sens[4] = +1.0;
            else if ( angleAile >= 90.0 ) sens[4] = -1.0;
            angleAile += 2.0 * sens[4];

            // taille du corps
            if ( taille <= 0.25 ) sens[5] = +1.0;
            else if ( taille >= 1.0 ) sens[5] = -1.0;
            taille += 0.005 * sens[5];

            // rotation du corps
            if ( angleCorps > 360.0 ) angleCorps -= 360.0;
            angleCorps += 0.35;
        }
    }

    // partie 2: utilisation de vbo et vao
    GLuint vao = 0;
    GLuint vboTheiereSommets = 0;
    GLuint vboTheiereConnec = 0;
    GLint locVertex = -1;
    GLint locColor = -1;

    glm::vec3 position;       // position courante du bonhomme
    GLfloat taille;           // facteur d'échelle du corps
    GLfloat angleCorps;       // angle de rotation (en degrés) du bonhomme
    GLfloat angleAile;        // angle de rotation (en degrés) des ailes
    GLfloat angleJambe;       // angle de rotation (en degrés) des jambes
    const GLfloat longMembre = 0.7;  // longueur des membres
    const GLfloat largMembre = 0.1;  // largeur des membres
};

#endif
