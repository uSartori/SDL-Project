#include "Line.h"
#include "Context.h"
#include "Transform.h"
#include <math.h>

Line::Line()
{
    //ctor
}

Line::~Line()
{
    //dtor
}

Line::Line(Point start, Point end, Color color)
{
    this->start = start;
    this->end = end;
    this->color = color;
    this->antialias = 1;
}

/*
Line::Line(Point start, Point end, Color color, int antialias) {
    this->start = start;
    this->end = end;
    this->color = color;
    this->antialias = antialias;
}
*/

void Line::setPixel(int x, int y, Uint32 cor)
{

    Color c = Color();
    Uint8 r = c.getColorComponent(cor, 'r');
    Uint8 g = c.getColorComponent(cor, 'g');
    Uint8 b = c.getColorComponent(cor, 'b');
    this->setPixel(x, y, r, g, b);
}

void Line::setPixel(int x, int y, Color color)
{
    this->setPixel(x, y, color.getR(),color.getG(),color.getB());
}

/*
void Line::setPixel(int x, int y, int r, int g, int b) {
    SDL_Renderer * pRenderer = Context::getInstance()->getRenderer();
	SDL_SetRenderDrawColor(pRenderer, r, g, b, 255);
	SDL_RenderDrawPoint(pRenderer, x, y);
}
*/

void Line::setPixel(int x, int y, int r, int g, int b)
{
    setPixel(x, y, r, g, b, 255);
}

// Pixels tiveram que ser ignorados para nao acessar uma posicao
// invalida da memoria resultando em segmentation fault (crash)
void Line::setPixel(int x, int y, int r, int g, int b, int a)
{
    SDL_Surface* window_surface =
        Context::getInstance()->getWindowSurface();

    if (x < 0 || x >= window_surface->w ||
            y < 0 || y >= window_surface->h)
    {
        return;
    }

    Context* context = Context::getInstance();

    if (!context->isInsideViewport(x, y))
    {
        return;
    }

    unsigned int* pixels =
        (unsigned int*)window_surface->pixels;

    pixels[x + y * window_surface->w] =
        SDL_MapRGBA(
            window_surface->format,
            r, g, b, a
        );
}

void Line::draw()
{
    if(this->antialias)
    {
        this->drawWuLine(
            this->start.getX(),
            this->start.getY(),
            this->end.getX(),
            this->end.getY(),
            this->color);
    }
    else
    {
        this->bresenham(
            this->start.getX(),
            this->start.getY(),
            this->end.getX(),
            this->end.getY(),
            this->color.getR(),
            this->color.getG(),
            this->color.getB());
    }
}

Uint32 Line::getPixel(int x, int y)
{
    SDL_Surface* window_surface = Context::getInstance()->getWindowSurface();

    if (x < 0 || x >= window_surface->w || y < 0 || y >= window_surface->h)
        return 0;

    int bpp = window_surface->format->BytesPerPixel;
    Uint8* p = (Uint8*)window_surface->pixels + y * window_surface->pitch + x * bpp;

    switch (bpp)
    {
    case 1:
        return *p;
    case 2:
        return *(Uint16*)p;
    case 3:
        if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
            return p[0] << 16 | p[1] << 8 | p[2];
        return p[0] | p[1] << 8 | p[2] << 16;
    case 4:
        return *(Uint32*)p;
    default:
        return 0;
    }
}

void Line::drawWuLine(int x0, int y0, int x1, int y1, Color color )
{

    /* Make sure the line runs top to bottom */
    if (y0 > y1)
    {
        int aux = y0;
        y0 = y1;
        y1 = aux;
        aux = x0;
        x0 = x1;
        x1 = aux;
    }

    /* Draw the initial pixel, which is always exactly intersected by
    the line and so needs no weighting */
    setPixel( x0, y0, color );

    int xDir, deltaX = x1 - x0;
    if( deltaX >= 0 )
    {
        xDir = 1;
    }
    else
    {
        xDir   = -1;
        deltaX = 0 - deltaX; /* make deltaX positive */
    }

    /* Special-case horizontal, vertical, and diagonal lines, which
    require no weighting because they go right through the center of
    every pixel */
    int deltaY = y1 - y0;
    if (deltaY == 0)
    {
        /* Horizontal line */
        while (deltaX-- != 0)
        {
            x0 += xDir;
            setPixel( x0, y0, color );
        }
        return;
    }
    if (deltaX == 0)
    {
        /* Vertical line */
        do
        {
            y0++;
            setPixel( x0, y0, color );
        }
        while (--deltaY != 0);
        return;
    }

    if (deltaX == deltaY)
    {
        /* Diagonal line */
        do
        {
            x0 += xDir;
            y0++;
            setPixel( x0, y0, color );
        }
        while (--deltaY != 0);
        return;
    }

    unsigned short errorAdj;
    unsigned short errorAccaux, weighting;

    /* Line is not horizontal, diagonal, or vertical */
    unsigned short errorAcc = 0;  /* initialize the line error accumulator to 0 */



    Uint32 rl = color.getR();
    Uint32 gl = color.getG();
    Uint32 bl = color.getB();
    double grayl = rl * 0.299 + gl * 0.587 + bl * 0.114;

    /* Is this an X-major or Y-major line? */
    if (deltaY > deltaX)
    {
        /* Y-major line; calculate 16-bit fixed-point fractional part of a
        pixel that X advances each time Y advances 1 pixel, truncating the
            result so that we won't overrun the endpoint along the X axis */
        errorAdj = ((unsigned long) deltaX << 16) / (unsigned long) deltaY;
        /* Draw all pixels other than the first and last */
        while (--deltaY)
        {
            errorAccaux = errorAcc;   /* remember currrent accumulated error */
            errorAcc += errorAdj;      /* calculate error for next pixel */
            if (errorAcc <= errorAccaux)
            {
                /* The error accumulator turned over, so advance the X coord */
                x0 += xDir;
            }
            y0++; /* Y-major, so always advance Y */
            /* The IntensityBits most significant bits of errorAcc give us the
            intensity weighting for this pixel, and the complement of the
            weighting for the paired pixel */
            weighting = errorAcc >> 8;
            /*
            ASSERT( weighting < 256 );
            ASSERT( ( weighting ^ 255 ) < 256 );
            */
            Uint32 clrBackGround = getPixel(x0, y0 );
            //clrBackGround = RGB(255, 255, 255);
            Uint8 rb = color.getColorComponent(clrBackGround,'r');
            Uint8 gb = color.getColorComponent(clrBackGround,'g');
            Uint8 bb = color.getColorComponent(clrBackGround,'b');
            double grayb = rb * 0.299 + gb * 0.587 + bb * 0.114;

            Uint8 rr = ( rb > rl ? ( ( Uint8 )( ( ( double )( grayl<grayb?weighting:(weighting ^ 255)) ) / 255.0 * ( rb - rl ) + rl ) ) : ( ( Uint8 )( ( ( double )( grayl<grayb?weighting:(weighting ^ 255)) ) / 255.0 * ( rl - rb ) + rb ) ) );
            Uint8 gr = ( gb > gl ? ( ( Uint8 )( ( ( double )( grayl<grayb?weighting:(weighting ^ 255)) ) / 255.0 * ( gb - gl ) + gl ) ) : ( ( Uint8 )( ( ( double )( grayl<grayb?weighting:(weighting ^ 255)) ) / 255.0 * ( gl - gb ) + gb ) ) );
            Uint8 br = ( bb > bl ? ( ( Uint8 )( ( ( double )( grayl<grayb?weighting:(weighting ^ 255)) ) / 255.0 * ( bb - bl ) + bl ) ) : ( ( Uint8 )( ( ( double )( grayl<grayb?weighting:(weighting ^ 255)) ) / 255.0 * ( bl - bb ) + bb ) ) );
            setPixel( x0, y0, color.getColor( rr, gr, br ) );

            clrBackGround = getPixel(x0 + xDir, y0 );
            //clrBackGround = RGB(255, 255, 255);
            rb = color.getColorComponent( clrBackGround, 'r' );
            gb = color.getColorComponent( clrBackGround, 'g' );
            bb = color.getColorComponent( clrBackGround, 'b' );
            grayb = rb * 0.299 + gb * 0.587 + bb * 0.114;

            rr = ( rb > rl ? ( ( Uint8 )( ( ( double )( grayl<grayb?(weighting ^ 255):weighting) ) / 255.0 * ( rb - rl ) + rl ) ) : ( ( Uint8 )( ( ( double )( grayl<grayb?(weighting ^ 255):weighting) ) / 255.0 * ( rl - rb ) + rb ) ) );
            gr = ( gb > gl ? ( ( Uint8 )( ( ( double )( grayl<grayb?(weighting ^ 255):weighting) ) / 255.0 * ( gb - gl ) + gl ) ) : ( ( Uint8 )( ( ( double )( grayl<grayb?(weighting ^ 255):weighting) ) / 255.0 * ( gl - gb ) + gb ) ) );
            br = ( bb > bl ? ( ( Uint8 )( ( ( double )( grayl<grayb?(weighting ^ 255):weighting) ) / 255.0 * ( bb - bl ) + bl ) ) : ( ( Uint8 )( ( ( double )( grayl<grayb?(weighting ^ 255):weighting) ) / 255.0 * ( bl - bb ) + bb ) ) );
            setPixel( x0 + xDir, y0, color.RGB( rr, gr, br ) );
        }
        /* Draw the final pixel, which is always exactly intersected by the line
        and so needs no weighting */
        setPixel( x1, y1, color );
        return;
    }
    /* It's an X-major line; calculate 16-bit fixed-point fractional part of a
    pixel that Y advances each time X advances 1 pixel, truncating the
    result to avoid overrunning the endpoint along the X axis */
    errorAdj = ((unsigned long) deltaY << 16) / (unsigned long) deltaX;
    /* Draw all pixels other than the first and last */
    while (--deltaX)
    {
        errorAccaux = errorAcc;   /* remember currrent accumulated error */
        errorAcc += errorAdj;      /* calculate error for next pixel */
        if (errorAcc <= errorAccaux)
        {
            /* The error accumulator turned over, so advance the Y coord */
            y0++;
        }
        x0 += xDir; /* X-major, so always advance X */
        /* The IntensityBits most significant bits of errorAcc give us the
        intensity weighting for this pixel, and the complement of the
        weighting for the paired pixel */
        weighting = errorAcc >> 8;
        /*
        ASSERT( weighting < 256 );
        ASSERT( ( weighting ^ 255 ) < 256 );
        */
        Uint32 clrBackGround = getPixel(x0, y0 );
        //clrBackGround = RGB(255, 255, 255);
        Uint8 rb = color.getColorComponent( clrBackGround, 'r' );
        Uint8 gb = color.getColorComponent( clrBackGround, 'g' );
        Uint8 bb = color.getColorComponent( clrBackGround, 'b' );
        double grayb = rb * 0.299 + gb * 0.587 + bb * 0.114;

        Uint8 rr = ( rb > rl ? ( ( Uint8 )( ( ( double )( grayl<grayb?weighting:(weighting ^ 255)) ) / 255.0 * ( rb - rl ) + rl ) ) : ( ( Uint8 )( ( ( double )( grayl<grayb?weighting:(weighting ^ 255)) ) / 255.0 * ( rl - rb ) + rb ) ) );
        Uint8 gr = ( gb > gl ? ( ( Uint8 )( ( ( double )( grayl<grayb?weighting:(weighting ^ 255)) ) / 255.0 * ( gb - gl ) + gl ) ) : ( ( Uint8 )( ( ( double )( grayl<grayb?weighting:(weighting ^ 255)) ) / 255.0 * ( gl - gb ) + gb ) ) );
        Uint8 br = ( bb > bl ? ( ( Uint8 )( ( ( double )( grayl<grayb?weighting:(weighting ^ 255)) ) / 255.0 * ( bb - bl ) + bl ) ) : ( ( Uint8 )( ( ( double )( grayl<grayb?weighting:(weighting ^ 255)) ) / 255.0 * ( bl - bb ) + bb ) ) );

        setPixel( x0, y0, color.RGB( rr, gr, br ) );

        clrBackGround = getPixel(x0, y0 + 1 );
        //clrBackGround = RGB(255, 255, 255);
        rb = color.getColorComponent( clrBackGround, 'r' );
        gb = color.getColorComponent( clrBackGround, 'g' );
        bb = color.getColorComponent( clrBackGround, 'b' );
        grayb = rb * 0.299 + gb * 0.587 + bb * 0.114;

        rr = ( rb > rl ? ( ( Uint8 )( ( ( double )( grayl<grayb?(weighting ^ 255):weighting) ) / 255.0 * ( rb - rl ) + rl ) ) : ( ( Uint8 )( ( ( double )( grayl<grayb?(weighting ^ 255):weighting) ) / 255.0 * ( rl - rb ) + rb ) ) );
        gr = ( gb > gl ? ( ( Uint8 )( ( ( double )( grayl<grayb?(weighting ^ 255):weighting) ) / 255.0 * ( gb - gl ) + gl ) ) : ( ( Uint8 )( ( ( double )( grayl<grayb?(weighting ^ 255):weighting) ) / 255.0 * ( gl - gb ) + gb ) ) );
        br = ( bb > bl ? ( ( Uint8 )( ( ( double )( grayl<grayb?(weighting ^ 255):weighting) ) / 255.0 * ( bb - bl ) + bl ) ) : ( ( Uint8 )( ( ( double )( grayl<grayb?(weighting ^ 255):weighting) ) / 255.0 * ( bl - bb ) + bb ) ) );

        setPixel( x0, y0 + 1, color.RGB( rr, gr, br ) );
    }

    /* Draw the final pixel, which is always exactly intersected by the line
    and so needs no weighting */
    setPixel( x1, y1, color );
}

void Line::bresenham(int x1, int y1, int x2, int y2, int r, int g, int b)
{
    int x,y,dx,dy,dx1,dy1,px,py,xe,ye,i;

    dx=x2-x1;
    dy=y2-y1;

    dx1=fabs(dx);
    dy1=fabs(dy);

    px=2*dy1-dx1;
    py=2*dx1-dy1;

    if(dy1<=dx1)
    {
        if(dx>=0)
        {
            x=x1;
            y=y1;
            xe=x2;
        }
        else
        {
            x=x2;
            y=y2;
            xe=x1;
        }
        setPixel(x,y,r,g,b);
        for(i=0; x<xe; i++)
        {
            x=x+1;
            if(px<0)
            {
                px=px+2*dy1;
            }
            else
            {
                if((dx<0 && dy<0) || (dx>0 && dy>0))
                {
                    y=y+1;
                }
                else
                {
                    y=y-1;
                }
                px=px+2*(dy1-dx1);
            }
            setPixel(x,y,r,g,b);
        }
    }
    else
    {
        if(dy>=0)
        {
            x=x1;
            y=y1;
            ye=y2;
        }
        else
        {
            x=x2;
            y=y2;
            ye=y1;
        }
        setPixel(x,y,r,g,b);
        for(i=0; y<ye; i++)
        {
            y=y+1;
            if(py<=0)
            {
                py=py+2*dx1;
            }
            else
            {
                if((dx<0 && dy<0) || (dx>0 && dy>0))
                {
                    x=x+1;
                }
                else
                {
                    x=x-1;
                }
                py=py+2*(dx1-dy1);
            }
            setPixel(x,y,r,g,b);
        }
    }
}
// Translada a linha
void Line::translate(double tx, double ty)
{
    Point points[2] = {start, end};

    Transform transform;
    transform.translate(points, 2, tx, ty);

    start = points[0];
    end = points[1];
}

// Escala a linha usando um ponto de referencia
void Line::scale(double sx, double sy, Point reference)
{
    Point points[2] = {start, end};

    Transform transform;
    transform.scale(points, 2, sx, sy, reference);

    start = points[0];
    end = points[1];
}

void Line::rotate(double angle, Point reference)
{
    Point points[2] = {start, end};

    Transform transform;
    transform.rotate(points, 2, angle, reference);

    start = points[0];
    end = points[1];
}

// Verifica se o clique está perto da linha
bool Line::isNear(int clickX, int clickY)
{
    double dx = end.getX() - start.getX();
    double dy = end.getY() - start.getY();
    double lengthSquared = dx * dx + dy * dy;

    if (lengthSquared == 0)
    {
        double diffX = clickX - start.getX();
        double diffY = clickY - start.getY();
        return diffX * diffX + diffY * diffY <= 25;
    }

    double t = ((clickX - start.getX()) * dx + (clickY - start.getY()) * dy) / lengthSquared;
    t = std::fmax(0.0, std::fmin(1.0, t));

    double closestX = start.getX() + t * dx;
    double closestY = start.getY() + t * dy;
    double distanceX = clickX - closestX;
    double distanceY = clickY - closestY;

    return distanceX * distanceX + distanceY * distanceY <= 25;
}

// Retorna o ponto inicial
Point Line::getStart()
{
    return start;
}

// Retorna o ponto final
Point Line::getEnd()
{
    return end;
}

void Line::setStart(Point start)
{
    this->start = start;
}

void Line::setEnd(Point end)
{
    this->end = end;
}
