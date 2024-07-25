#include "SDL.h"
#include "SDL_image.h"
#include <string>

using namespace std;

int main( int argc, char *argv[] ) {
    SDL_Init( SDL_INIT_VIDEO );
    IMG_Init( IMG_INIT_PNG );
    SDL_SetHintWithPriority( SDL_HINT_RENDER_DRIVER, "direct3d11", SDL_HINT_OVERRIDE );
    SDL_Window *window = SDL_CreateWindow( "Testing", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                             1200, 600, SDL_WINDOW_RESIZABLE );
    SDL_Renderer *renderer = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED );
    SDL_RendererInfo *rendererInfo = new SDL_RendererInfo();
    SDL_RendererInfo *driverInfo = new SDL_RendererInfo();
    SDL_GetRendererInfo( renderer, rendererInfo );
    printf("%s", rendererInfo->name);
    int drivers = SDL_GetNumRenderDrivers();
    string availableDrivers = " (";
    for ( int i = 0; i < drivers; ++i ) {
        SDL_GetRenderDriverInfo( i, driverInfo );
        string driverName = driverInfo->name;
        if ( i == drivers - 1 ) {
            availableDrivers += driverName;
        }
        else {
            availableDrivers += driverName + ", ";
        }
    }
    availableDrivers += ")";
    string path = SDL_GetBasePath();
    SDL_Surface *surfRed = IMG_Load( (path + "\\Red.png").c_str() );
    SDL_Texture *textRed = SDL_CreateTextureFromSurface( renderer, surfRed );
    SDL_FreeSurface( surfRed );
    SDL_Surface *surfBlue = IMG_Load( ( path + "\\Blue.png" ).c_str() );
    SDL_Texture *textBlue = SDL_CreateTextureFromSurface( renderer, surfBlue );
    SDL_FreeSurface( surfBlue );
    SDL_Rect destRed, destBlue;
    destRed.x = 128;
    destRed.y = 128;
    destBlue.x = 196;
    destBlue.y = 196;
    SDL_QueryTexture( textRed, NULL, NULL, &destRed.w, &destRed.h );
    SDL_QueryTexture( textBlue, NULL, NULL, &destBlue.w, &destBlue.h );
    SDL_BlendMode blendMode = SDL_ComposeCustomBlendMode( SDL_BLENDFACTOR_ONE, SDL_BLENDFACTOR_ONE, SDL_BLENDOPERATION_MAXIMUM,
        SDL_BLENDFACTOR_ONE, SDL_BLENDFACTOR_ONE, SDL_BLENDOPERATION_MAXIMUM );         
    SDL_SetTextureBlendMode( textRed, blendMode );
    SDL_SetTextureBlendMode( textBlue, blendMode );
//  SDL_SetRenderDrawBlendMode( renderer, blendMode );
    string info = rendererInfo->name + availableDrivers + " " + SDL_GetError();
    SDL_SetWindowTitle( window, info.c_str() );
    SDL_SetRenderDrawColor( renderer, 0, 0, 0, 255 );
    SDL_Event event;
    bool isRunning = true;
    while ( isRunning ) {
        if ( SDL_PollEvent( &event ) ) {
            if ( event.type == SDL_QUIT ) {
                isRunning = false;
            }
        }
        SDL_RenderClear( renderer );
        SDL_RenderCopy( renderer, textRed, NULL, &destRed );
        SDL_RenderCopy( renderer, textBlue, NULL, &destBlue );
        SDL_RenderPresent( renderer );
    }
    delete driverInfo;
    delete rendererInfo;
    SDL_DestroyTexture( textRed );
    SDL_DestroyTexture( textBlue );
    SDL_DestroyRenderer( renderer );
    SDL_DestroyWindow( window );
    IMG_Quit();
    SDL_Quit();
    return 0;
}