#define SDL_MAIN_HANDLED
#include<SDL2/SDL.h>
#include<stdio.h>
#include <unistd.h>

#include<Context.h>
#include<Color.h>
#include<Point.h>
#include<Line.h>
#include<Circle.h>
#include<Curve.h>
#include<Polygon.h>
#include<Rectangle.h>
#include <vector>
#include <Toolbar.h>

// SDL stuff
SDL_Window* pWindow = nullptr;
SDL_Renderer* pRenderer = nullptr;
SDL_Surface * window_surface = nullptr;

int offset = 10;

std::vector<Shape*> shapes; //lista de formas

void display(Toolbar& toolbar)
{
    for(Shape* s : shapes) {
       if(s != nullptr) s->draw();
    }

    toolbar.render();

    /*// Os desenhos e transformações abaixo são apenas exemplos para testar
    // as classes. O próximo passo é substituir essas chamadas fixas por
    // interações com o mouse, permitindo selecionar uma figura e aplicar
    // translação, escala e rotação através dos comandos do usuário.
    // Para isso, pode ser utilizado um Shape* para trabalhar com qualquer
    // tipo de figura (Circle, Curve, Polygon, Rectangle, etc.).

    Color red = Color(255, 0, 0);
    Color blue = Color(0, 0, 255);
    Color green = Color(0, 255, 0);

    Circle cir = Circle(Point(200, 200), 90, red);

    Point curvePoints[] = {
        Point(100, 200),
        Point(200, 100),
        Point(150, 150),
        Point(300, 400)
    };

    Curve cur = Curve(curvePoints, green);

    list<Point> pontos;

    pontos.push_back(Point(100, 100));
    pontos.push_back(Point(300, 100));
    pontos.push_back(Point(300, 300));
    pontos.push_back(Point(100, 300));

    Polygon pol = Polygon(pontos, blue);

    Rectangle rect = Rectangle(Point(300, 300), 300, 150, red);

    cir.draw();
    cur.draw();
    pol.draw();
    rect.draw();

    cir.translate(100, 50);

    cur.scale(0.5, 0.5);

    pol.rotate(45);

    rect.translate(-100, -50);
    rect.scale(0.5, 0.5);
    rect.rotate(30);

    cir.draw();
    cur.draw();
    pol.draw();
    rect.draw();*/

    // Codigo que veio do professor (pode ser util)
    /*
    Point p1 = Point(600-offset,400);
    Point p2 = Point(10+offset,10);

    Line l1 = Line(p1, p2, color);
    l1.draw();

    Point p3 = Point(600-offset,430);
    Point p4 = Point(10+offset,40);

    offset += 10;

    if(offset > 500) {
        offset = 0;
    }

    Line l2 = Line(p3, p4, color, 1);
    l2.draw();

    */
}

void clear() {

    Line l = Line();
    SDL_Surface * window_surface = Context::getInstance()->getWindowSurface();
    for(int x = 0;x < window_surface->w; x++){
        for(int y = 0;y < window_surface->h; y++) {
            l.setPixel(x, y, 255, 255, 255);
        }
    }

}

// Driver code
int main(int argc, char* args[])
{

	SDL_Event event;

	// initialize SDL
	if (SDL_Init(SDL_INIT_EVERYTHING) >= 0)
	{
		// if succeeded create our window
		pWindow = SDL_CreateWindow("SDL_Classes",
					SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
														640, 480,
												SDL_WINDOW_SHOWN);

		// if the window creation succeeded create our renderer
		if (pWindow != 0) {
			pRenderer = SDL_CreateRenderer(pWindow, -1, 0);
			window_surface = SDL_GetWindowSurface(pWindow);

            // Inicializa o contexto gráfico da aplicação
			Context * context = Context::getInstance();
			context->setRenderer(pRenderer);
			context->setWindowSurface(window_surface);

        }

	}
	else
		return 1; // sdl could not initialize

    Toolbar toolbar(640, 40);

	while (1)
	{

        // Limpa a tela
        clear();

        // Realiza o desenho
        display(toolbar);

        // Aguarda instantes
        usleep(100000);

		// Verifica se foi mandado fechar a janela
		while (SDL_PollEvent(&event))
        {

            if (event.type == SDL_QUIT)
            {
                exit(0);
            }

            bool clickedUI = toolbar.handleEvent(event);

           /*adiiconar depois
             Se clickedUI for falso, o clique ocorreu na área livre
             utilizar toolbar.getCurrentTool() p identificar a ferramenta ativa
             e instanciar dinamicamente os objetos
             salvando os ponteiros no vetor 'shapes'
             */

        }

		// Atualiza a tela
		SDL_UpdateWindowSurface(pWindow);
	}

	// clean up SDL
	SDL_Quit();
	return 0;
}
