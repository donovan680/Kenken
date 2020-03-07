#include <stdio.h>
#include <SDL2/SDL.h>
#include "kenken.c"
#include "SDL2/SDL_ttf.h"

//Screen dimension constants
const int SCREEN_WIDTH = 720;
const int SCREEN_HEIGHT = 600;
const int MARGIN = 30;
const int SQR_SIZE = 72;
const int SOLUTION_BOX = 15;
const int SMALL_FONT = 14;
const int BIG_FONT = 60;

int update_edge_arrays(int vertedge[5][6], int horiedge[6][5], struct constraint *cstr);

int set_kenken_boundaries(int vertedge[5][6], int horiedge[6][5], struct kenken* kenkenptr);

int draw_function(SDL_Window *window, SDL_Renderer *renderer, SDL_Rect *text_rect_ptr, SDL_Point corners[5], SDL_Point points[49], SDL_Rect rects[36], SDL_Rect *selected_sqr, int vertedge[5][6], int horiedge[6][5]);

struct node_ctrdraw{
	int result;				//the result of the operation
	int op;					//the operation: defined by preprocessor in kenken.c
	int topleft[2];			//the position of the topleftmost square
	struct node_ctrdraw *next_node;
};

int draw_corner_number_textures(SDL_Renderer *renderer, struct node_ctrdraw *tlhead, SDL_Texture *textrects[36], SDL_Rect corner_numbers[36]);

int draw_central_number_textures(SDL_Renderer *renderer, SDL_Texture *num_texts[6], int txtboxdims[6][2]);

int draw_central_numbers(SDL_Renderer *renderer, SDL_Texture *num_texts[6], SDL_Rect rects[36], int txtboxdims[6][2], int usrgrid[6][6]);

//int draw_constraint_corners();

int main( int argc, char* args[] )
{
    //The window we'll be rendering to
    SDL_Window* window = NULL;
    
    //The surface contained by the window
    SDL_Surface* screenSurface = NULL;
	
	//SDL_Texture* texture = NULL;
	
	SDL_Renderer* renderer = NULL;

    //Initialize SDL
    if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
    {
        printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
    }
    else
    {
        //Create window
        window = SDL_CreateWindow("Potato", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if( window == NULL )
        {
            printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        }
        else
        {
			renderer = SDL_CreateRenderer(window, -1, 0);
			//SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 0);
            //Get window surface
            //screenSurface = SDL_GetWindowSurface( window );
			if(TTF_Init()==-1) {
    			printf("TTF_Init: %s\n", TTF_GetError());
   		 		exit(2);
			}
			
			TTF_Font *font2;
			font2 = TTF_OpenFont("OpenSans-Regular.ttf", BIG_FONT);
			/*Fill the surface white
            SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0xFF, 0xFF, 0xFF));
			texture = SDL_CreateTextureFromSurface(renderer, screenSurface);
			//SDL_FreeSurface(screenSurface);*/
			SDL_Rect texture_rect;
			texture_rect.x = 0;  //the x coordinate
			texture_rect.y = 0; // the y coordinate
			texture_rect.w = SCREEN_WIDTH; //the width of the texture
			texture_rect.h = SCREEN_HEIGHT; //the height of the texture
			
			
			//Update the surface
            //SDL_UpdateWindowSurface( window );		
			
			SDL_RenderSetScale(renderer, 1.0, 1.0);
			
			
			
			SDL_Point points[49];
			
			for(int i = 0; i < 49; i++){
				points[i].x = MARGIN + SQR_SIZE*(i%7);
				points[i].y = MARGIN + SQR_SIZE*(i/7);
			}
			
			SDL_Rect rects[36];
			
			for(int i = 0; i < 36; i++){
				rects[i].x = MARGIN + SQR_SIZE*(i%6)+1;
				rects[i].y = MARGIN + SQR_SIZE*(i/6)+1;
				rects[i].w = SQR_SIZE-1;
				rects[i].h = SQR_SIZE-1;
			}
			
			SDL_Rect corner_numbers[36]; //rect for displaying puzzle clues
			
			for(int i = 0; i < 36; i++){
				corner_numbers[i].x = MARGIN + SQR_SIZE*(i%6)+1;
				corner_numbers[i].y = MARGIN + SQR_SIZE*(i/6)+1;
				corner_numbers[i].w = 1;
				corner_numbers[i].h = 1;
			}
			
			//textures for the corner numbers
			SDL_Texture *textrects[36];
			for(int i = 0; i < 36; i++) textrects[i] = NULL;
			
			//textures for user entered numbers
			SDL_Texture *num_texts[6] = {NULL, NULL, NULL, NULL, NULL, NULL};
			
			//array containing num_texts dimensions
			int txtboxdim[6][2];
			
			draw_central_number_textures(renderer, num_texts, txtboxdim);
			
			struct kenken game;
			
			generate_kenken(&game);
			
			int vertedge[5][6];
			
			int horiedge[6][5];
			
			for(int i = 0; i < 6; i++){
				for(int j = 0; j < 5; j++){
					vertedge[j][i] = 1;
					horiedge[i][j] = 1;
				}
			}
			
			struct node_ctrdraw *tlhead = NULL; //list of topleft squares of the constraints
			
			for(struct node_ctr *dmy = game.ctrs; dmy != NULL; dmy = dmy->next_node){
				struct node_ctrdraw *element = malloc(sizeof(struct node_ctrdraw));
				element->next_node = tlhead;
				element->result = dmy->constraint.result;
				element->op = dmy->constraint.op;
				element->topleft[0] = 5;
				element->topleft[1] = 5;
				
				for(struct node_square *dmy2 = dmy->constraint.numbers; dmy2 != NULL; dmy2 = dmy2->next_node){
					if(element->topleft[0] > dmy2->pos[0]){
						element->topleft[0] = dmy2->pos[0];
						element->topleft[1] = dmy2->pos[1];
					}
					else if(element->topleft[0] == dmy2->pos[0] && element->topleft[1] > dmy2->pos[1]){
						element->topleft[1] = dmy2->pos[1];
					}
				}
				
				tlhead = element;
			}
			
			int usrgrid[6][6];
			
			for(int i = 0; i < 6; i++){
				for(int j = 0; j < 6; j++){
					usrgrid[i][j] = 0;
				}
			}
			
			draw_corner_number_textures(renderer, tlhead, textrects, corner_numbers);
			
			set_kenken_boundaries(vertedge, horiedge, &game);
			
			SDL_Point corners[5] = {points[0], points[6], points[48], points[42], points[0]};
			
			//SDL_RenderCopy(renderer, texture, NULL, &texture_rect);
			//SDL_SetRenderTarget(renderer, texture);

            //Exit event
		    SDL_Event e;
			
		    int quit = 0;
			
			SDL_Rect *selected_sqr = &rects[0];
			
			int selected_index = 0;
			
			//SDL_RenderPresent(renderer);
			
			while(!quit)
		    {
                 SDL_WaitEvent( &e );
		         
		         if( e.type == SDL_QUIT )
		         {
		                  quit = 1;
						  break;
		         }
				 if( e.type == SDL_MOUSEBUTTONUP ){
				 	for(int i = 0; i < 36; i++){
				 		if( e.button.x > rects[i].x && e.button.x < rects[i].x + rects[i].w && e.button.y > rects[i].y && e.button.y < rects[i].y + rects[i].h){
				 			
							if((*selected_sqr).x == rects[i].x && (*selected_sqr).y == rects[i].y) break;
							
							//SDL_SetRenderDrawColor(renderer, 210, 210, 250, SDL_ALPHA_OPAQUE);
							
							selected_sqr = &rects[i];
							
							selected_index = i;

				 		}
				 	}
				}
				if( e.type == SDL_KEYDOWN){
					switch(e.key.keysym.sym){
						case SDLK_UP:
						if(selected_index > 5){
							selected_index = selected_index - 6;
							selected_sqr = &rects[selected_index];
						}
						break;
						case SDLK_DOWN:
						if(selected_index < 30){
							selected_index = selected_index + 6;
							selected_sqr = &rects[selected_index];
						}
						break;
						case SDLK_LEFT:
						if(selected_index % 6 != 0){
							selected_index = selected_index - 1;
							selected_sqr = &rects[selected_index];
						}
						break;
						case SDLK_RIGHT:
						if(selected_index % 6 != 5){
							selected_index = selected_index + 1;
							selected_sqr = &rects[selected_index];
						}
						break;
						case SDLK_BACKSPACE:
						usrgrid[selected_index%6][selected_index/6] = 0;
						break;
						case SDLK_1:
						usrgrid[selected_index%6][selected_index/6] = 1;
						break;
						case SDLK_2:
						usrgrid[selected_index%6][selected_index/6] = 2;
						break;
						case SDLK_3:
						usrgrid[selected_index%6][selected_index/6] = 3;
						break;
						case SDLK_4:
						usrgrid[selected_index%6][selected_index/6] = 4;
						break;
						case SDLK_5:
						usrgrid[selected_index%6][selected_index/6] = 5;
						break;
						case SDLK_6:
						usrgrid[selected_index%6][selected_index/6] = 6;
						break;
					}
					
					//update_usr_kenken(usrgrid, usrkenken)
				
				}
				
				//SDL_RenderClear(renderer);
			
	 			//SDL_RenderCopy(renderer, texture, NULL, &texture_rect);
				
				draw_function(window, renderer, &texture_rect, corners, points, rects, selected_sqr, vertedge, horiedge);
				
				for(int i = 0; i < 36; i++){
					if(textrects[i] != NULL){
						SDL_RenderCopy(renderer, textrects[i], NULL, &corner_numbers[i]);
					}
				}
				
				draw_central_numbers(renderer, num_texts, rects, txtboxdim, usrgrid);
				
				SDL_RenderPresent(renderer);
		    }
        }
    }
	//Destroy renderer
	SDL_DestroyRenderer(renderer);
	
	//TTF_CloseFont(font2);
	
	TTF_Quit();
	
    //Destroy window
    SDL_DestroyWindow( window );
	
	

    //Quit SDL subsystems
    SDL_Quit();

    return 0;
}

int draw_function(SDL_Window *window, SDL_Renderer *renderer, SDL_Rect *text_rect_ptr, SDL_Point corners[5], SDL_Point points[49], SDL_Rect rects[36], SDL_Rect *selected_sqr, int vertedge[5][6], int horiedge[6][5]){
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
	SDL_RenderFillRect(renderer, text_rect_ptr);
	
	SDL_SetRenderDrawColor(renderer, 245, 245, 245, SDL_ALPHA_OPAQUE);
	SDL_RenderFillRects(renderer, rects, 36);
	
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
	SDL_RenderDrawPoints(renderer, points, 49);
	SDL_RenderDrawLines(renderer, corners, 5);
	
	for(int i = 0; i < 6; i++){
		for(int j = 0; j < 5; j++){
			if(vertedge[j][i]){
				SDL_RenderDrawLine(renderer, points[i*7 + (j+1)].x, points[i*7 + (j+1)].y, points[(i+1)*7 + (j+1)].x, points[(i+1)*7 + (j+1)].y);
			}
			if(horiedge[i][j]){
				SDL_RenderDrawLine(renderer, points[(j+1)*7 + i].x, points[(j+1)*7 + i].y, points[(j+1)*7 + (i+1)].x, points[(j+1)*7 + (i+1)].y);
			}
		}
	}
	
	SDL_SetRenderDrawColor(renderer, 210, 210, 250, SDL_ALPHA_OPAQUE);
	SDL_RenderFillRect(renderer, selected_sqr);
	
	return 0;
}

int draw_corner_number_textures(SDL_Renderer *renderer, struct node_ctrdraw *tlhead, SDL_Texture *textrects[36], SDL_Rect corner_numbers[36]){
	//Set font/size/other key features
	SDL_Color colour = {0, 0, 0};
	TTF_Font *font;
	font = TTF_OpenFont("OpenSans-Regular.ttf", SMALL_FONT);
	//traverse list, draw each number
	//printf("Got to here\n");
	for(struct node_ctrdraw *dmy = tlhead; dmy != NULL; dmy = dmy->next_node){
		//draw number on transparent background
		SDL_Surface *text = NULL;
		char numresult[6];
		char operation;
		//printf("Got to here %d\n", (int) dmy);
		if(dmy->op == ADDOP) operation = '+';
		else if(dmy->op == SUBOP) operation = '-';
		else if(dmy->op == DIVOP) operation = '/';
		else if(dmy->op == MULTOP) operation = '*';
		else if(dmy->op == NOOP) operation = ' ';
		if(operation == ' ') SDL_snprintf(numresult, 6, "%c%d", operation, dmy->result); //Overflow??
		else SDL_snprintf(numresult, 6, " %c%d", operation, dmy->result); //Overflow??
		text = TTF_RenderText_Blended(font, numresult, colour);
		int ind = (dmy->topleft)[1]*6 + (dmy->topleft)[0];
		//printf("%s\n", numresult);
		//blit to correct screen location, given by dmy->topleft[0] and [1]
		//SDL_BlitSurface(text, &rects[ind], textrects[ind], &rects[ind]);
		textrects[ind] = SDL_CreateTextureFromSurface(renderer, text);
		corner_numbers[ind].w = text->w;
		corner_numbers[ind].h = text->h;
		SDL_FreeSurface(text);
	}
	TTF_CloseFont(font);
	return 0;
}

int draw_central_number_textures(SDL_Renderer *renderer, SDL_Texture *num_texts[6], int txtboxdims[6][2]){
	//Set font/size/other key features
	SDL_Color colour = {0, 0, 0};
	TTF_Font *font;
	font = TTF_OpenFont("OpenSans-Regular.ttf", BIG_FONT);
	for(int i = 0; i<6; i++){
		SDL_Surface *text = NULL;
		char num[5];
		SDL_snprintf(num, 5, "%d", i+1);
		text = TTF_RenderText_Blended(font, num, colour);
		//printf("%s\n", num);
		num_texts[i] = SDL_CreateTextureFromSurface(renderer, text);
		txtboxdims[i][0] = text->w;
		txtboxdims[i][1] = text->h;
		SDL_FreeSurface(text);
	}
	TTF_CloseFont(font);
	return 0;
}

int draw_central_numbers(SDL_Renderer *renderer, SDL_Texture *num_texts[6], SDL_Rect rects[36], int txtboxdims[6][2], int usrgrid[6][6]){
	for(int i = 0; i < 6; i++){
		for(int j = 0; j < 6; j++){
			if(usrgrid[i][j] < 7 && usrgrid[i][j] > 0){
				SDL_Rect txtbox;
				int centrex = rects[6*j + i].x + rects[6*j + i].w/2;
				int centrey = rects[6*j + i].y + rects[6*j + i].h/2;
				txtbox.x = centrex - txtboxdims[usrgrid[i][j]-1][0]/2;
				txtbox.y = centrey - txtboxdims[usrgrid[i][j]-1][1]/2;
				txtbox.w = txtboxdims[usrgrid[i][j] - 1][0];
				txtbox.h = txtboxdims[usrgrid[i][j] - 1][1];
				SDL_RenderCopy(renderer, num_texts[usrgrid[i][j] - 1], NULL, &txtbox);
			}
		}
	}
	return 0;
}

//vertedge and horiedge are indexed according to the square above/left of them
int update_edge_arrays(int vertedge[5][6], int horiedge[6][5], struct constraint *cstr){
	for(struct node_square *ele1 = cstr->numbers; ele1 != NULL; ele1 = ele1->next_node){
		for(struct node_square *ele2 = ele1->next_node; ele2 != NULL; ele2 = ele2->next_node){
			if(ele1->pos[0] == ele2->pos[0] && ele1->pos[1] == ele2->pos[1] - 1){
				horiedge[ele1->pos[0]][ele1->pos[1]] = 0;
			}
			if(ele1->pos[0] == ele2->pos[0] && ele1->pos[1] == ele2->pos[1] + 1){
				horiedge[ele1->pos[0]][ele2->pos[1]] = 0;
			}
			if(ele1->pos[0] == ele2->pos[0] - 1 && ele1->pos[1] == ele2->pos[1]){
				vertedge[ele1->pos[0]][ele2->pos[1]] = 0;
			}
			if(ele1->pos[0] == ele2->pos[0] + 1 && ele1->pos[1] == ele2->pos[1]){
				vertedge[ele2->pos[0]][ele2->pos[1]] = 0;
			}
		}
	}
	return 0;
}

//function to update the arrays of vertical and horizontal edges to be drawn
int set_kenken_boundaries(int vertedge[5][6], int horiedge[6][5], struct kenken* kenkenptr){
	for(struct node_ctr *dmy = kenkenptr->ctrs; dmy != NULL; dmy = dmy->next_node){
		update_edge_arrays(vertedge, horiedge, &(dmy->constraint));
	}
	return 0;
}