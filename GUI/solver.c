#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>


//-------------------------SOLVER--------------------------------

int empty_line(int k, int grid[9][9],int line)
{
	for(int j = 0; j < 9; j++)
	{
		//Check if the digit is already in a previous case of the line
		if (grid[line][j] == k)
		{
			return 0;
		}
	}
	return 1;
}

int empty_column(int k, int grid[9][9],int column)
{
	for(int i = 0; i < 9; i++)
	{
		
		//Check if the digit is already in a previous case of the column
		if (grid[i][column] == k)
		{
			return 0;
		}
	}
	return 1;
}
int empty_square(int k, int grid[9][9],int line, int column)
{
	int _line = 3*(line/3);
	int _column = 3*(column/3);
	for(line = _line; line < _line+3; line++)
	{
		for(column = _column; column < _column+3; column++)
		{
			
			//Check if the digit is already in a previous case of the square
			if(grid[line][column] == k)
			{
				return 0;
			}
		}
	}
	return 1;
}

int isValid(int grid[9][9], int pos)
{
	//The grid is crossed
	if (pos == 9*9)
	{
		return 1;
	}

	int line = pos/9;
	int column = pos%9;

	//The case is not empty
	if (grid[line][column] != 0)
	{
		return isValid(grid,pos+1);
	}
	//Check the different digits
	for(int k = 1; k <= 9; k++)
	{
		//Check if the digit is allowed on the line, the column and the square
		if (empty_line(k,grid,line) == 1 && empty_column(k,grid,column) == 1 && empty_square(k,grid,line,column) == 1)
		{
			grid[line][column] = k;
			//Check if the next case has a solution 
			if(isValid(grid,pos+1))
			{
				return 1;
			}
		}
	}
	//None of the digit is valid 
	grid[line][column] = 0;
	return 0;
}


//-------------------------END_OF_SOLVER-------------------------------


//-------------------------FUNCTIONS_FOR_FILE-------------------------------


void open_my_file(char *filename,int grid[9][9])
{
	FILE *file;
	file = fopen(filename,"r");
	if (file == NULL)
	{
		errx(1,"Problem to open the file");
	}
	char c = fgetc(file);
	int pos = 0;
	int line = pos/9;
	int column = pos%9;
	while(c!=EOF && pos < 9*9)
	{
		line = pos/9;
		column = pos%9;
		//If the char is not a digit, it is replaced by a zero
		if ( c == '.' )
		{
			grid[line][column] = 0;
			pos+=1;
		}
		//If the char is a digit, it is added to the board
		else if (c != '\n' && c!= ' ')
		{
			grid[line][column] = c-48;
			pos+=1;
		}
		c = fgetc(file);
	}
	fclose(file);
}

void my_final_file (char *filename,int grid[9][9])
{
	FILE *res;
	char *name = strcat(filename,".result");
	res = fopen(name,"w");
	if (res == NULL)
	{
		errx(2,"Problem to open the file");
	}
	int pos = 0;
	int line = pos/9;
	int column = pos%9;
	while(pos < 9*9)
	{
		line = pos/9;
		column = pos%9;

		fputc(grid[line][column]+48,res);
		if(column == 8)
		{
			fputc('\n',res);

		}
		else if ((column+1)%3 == 0)
		{
			fputc(' ',res);
		}
		if(pos+1 < 9*9 &&(line+1)%3 == 0 && column == 8)
		{
			fputc('\n',res);
		}
		pos+=1;
	}
	fclose(res);
}

void print_my_grid(int grid[9][9])
{
	for(int i = 0; i < 9; i++)
	{
		for (int j = 0; j < 9; j++)
		{

			if((i+1)%3==0 && j == 8)
			{

				printf("%i\n",grid[i][j]);
			}
			else if((j+1)%3 == 0)
			{

				printf("%i ",grid[i][j]);
			}
			else
			{

				printf("%i",grid[i][j]);
			}
		}
		putchar('\n');
	}
}


//-------------------------END_OF_FUNCTIONS_FOR_FILE-------------------------------


/*int main(int argc, char **argv)
{
	if (argc != 2)
	{
		errx(3,"Too many or not enough arguments");
	}
	int grid[9][9];
	open_my_file(argv[1],grid);
	isValid(grid,0);
	//print_my_grid(grid);
	my_final_file(argv[1],grid);

	return 0;
}*/
