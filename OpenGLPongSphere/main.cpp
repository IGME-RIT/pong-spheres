

#include <iostream>
#include <vector>

// We are using the glew32s.lib
// Thus we have a define statement
// If we do not want to use the static library, we can include glew32.lib in the project properties
// If we do use the non static glew32.lib, we need to include the glew32.dll in the solution folder
// The glew32.dll can be found here $(SolutionDir)\..\External Libraries\GLEW\bin\Release\Win32
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Transform.h"
#include "ShaderProgram.h"



// Variables for the Height and width of the window
const GLint WIDTH = 800, HEIGHT = 800;
const GLfloat ballVelocity = 1.0f;
const GLfloat playerVelocity = 1.5f;
glm::vec3 ballDirection = glm::normalize(glm::vec3(1, -1, 0));


GLuint VAO;
GLuint VBO;

//Move a paddle based on input
int MovePlayer(GLFWwindow* window, int pNum)
{
	if (pNum == 1)
	{
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		{
			return 1;
		}
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		{
			return -1;
		}

		return 0;

	}

	if (pNum == 2)
	{
		if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		{
			return 1;
		}
		if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		{
			return -1;
		}

		return 0;
	}

}


//A basic 2D circle
struct BoundingCircle
{
	glm::vec3 center;
	GLfloat radius;

	BoundingCircle(glm::vec3 c, GLfloat r)
	{
		center = c;
		radius = r;
	}
};

//Circle-Circle collision
bool CircleCollision(BoundingCircle circle1, BoundingCircle circle2)
{
	GLfloat distance = glm::distance(circle1.center,circle2.center);

	GLfloat radiusSum = circle1.radius + circle2.radius;


	if (distance< radiusSum)
		return true;

	return false;
}

void ResolveCollision(Transform* player, Transform* ball)
{
	//Collision with player 1
	if (ball->position.x < 0)
	{
		//Check if the ball is behind or in front of the paddle
		if (ball->position.x > player->position.x)
			ballDirection = glm::normalize(glm::reflect((player->position - ball->position), glm::vec3(1, 0, 0)));

		return;
	}

	//Collision with player 2

	//Check if the ball is behind or in front of the paddle
	if (ball->position.x < player->position.x)
		ballDirection = glm::normalize(glm::reflect((player->position - ball->position), glm::vec3(-1, 0, 0)));
}
int main()
{
#pragma region GL setup
	//Initializes the glfw
	glfwInit();

	// Setting the required options for GLFW

	// Setting the OpenGL version, in this case 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_SAMPLES, 99);
	// Setting the Profile for the OpenGL.

	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

	// Setting the forward compatibility of the application to true
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	// We don't want the window to resize as of now.
	// Therefore we will set the resizeable window hint to false.
	// To make is resizeable change the value to GL_TRUE.
	//glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	// Create the window object
	// The first and second parameters passed in are WIDTH and HEIGHT of the window we want to create
	// The third parameter is the title of the window we want to create
	// NOTE: Fourth paramter is called monitor of type GLFWmonitor, used for the fullscreen mode.
	//		 Fifth paramter is called share of type GLFWwindow, here we can use the context of another window to create this window
	// Since we won't be using any of these two features for the current tutorial we will pass nullptr in those fields
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Shaders Tutorial", nullptr, nullptr);

	// We call the function glfwGetFramebufferSize to query the actual size of the window and store it in the variables.
	// This is useful for the high density screens and getting the window size when the window has resized.
	// Therefore we will be using these variables when creating the viewport for the window
	int screenWidth, screenHeight;
	glfwGetFramebufferSize(window, &screenWidth, &screenHeight);

	// Check if the window creation was successful by checking if the window object is a null pointer or not
	if (window == nullptr)
	{
		// If the window returns a null pointer, meaning the window creation was not successful
		// we print out the messsage and terminate the glfw using glfwTerminate()
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();

		// Since the application was not able to create a window we exit the program returning EXIT_FAILURE
		return EXIT_FAILURE;
	}

	// Creating a window does not make it the current context in the windows.
	// As a results if the window is not made the current context we wouldn't be able the perform the operations we want on it
	// So we make the created window to current context using the function glfwMakeContextCurrent() and passing in the Window object
	glfwMakeContextCurrent(window);

	// Enable GLEW, setting glewExperimental to true.
	// This allows GLEW take the modern approach to retrive function pointers and extensions
	glewExperimental = GL_TRUE;

	// Initialize GLEW to setup OpenGL function pointers
	if (GLEW_OK != glewInit())
	{
		// If the initalization is not successful, print out the message and exit the program with return value EXIT_FAILURE
		std::cout << "Failed to initialize GLEW" << std::endl;

		return EXIT_FAILURE;
	}

	// Setting up the viewport
	// First the parameters are used to set the top left coordinates
	// The next two parameters specify the height and the width of the viewport.
	// We use the variables screenWidth and screenHeight, in which we stored the value of width and height of the window,
	glViewport(0, 0, screenWidth, screenHeight);
#pragma endregion

#pragma region Game_Setup
	float ballWidth = 0.02f;
	Transform* player1 = new Transform();
	Transform* player2 = new Transform();

	player1->scale = player2->scale = glm::vec3(ballWidth, ballWidth*4, 1.0);
	player1->position = glm::vec3(-0.8f, 0.0, 0.0);
	player2->position = glm::vec3(0.8f, 0.0, 0.0);
	player1->Update();
	player2->Update();

	Transform* ball = new Transform();
	ball->position = glm::vec3(0.0, 0.8, 0.0);
	ball->scale = glm::vec3(ballWidth, ballWidth, 1.0);
	ball->Update();

	glm::vec2 score = glm::vec2(0, 0);

	//Each paddle will have 4 bounding circles 
	std::vector<BoundingCircle> p1Circs;
	std::vector<BoundingCircle> p2Circs;

	glm::vec3 circleCenter = glm::vec3(0, 0, 0);
	//Place 3 bounding circle on each paddle ( top middle bottom)
	circleCenter.y += ballWidth * 1.5f;						
	for (int i = 0; i < 4; i++)
	{
		circleCenter.x = player1->position.x;
		p1Circs.push_back(BoundingCircle(circleCenter, ballWidth));
		circleCenter.x = player2->position.x;
		p2Circs.push_back(BoundingCircle(circleCenter, ballWidth));
		circleCenter.y -= ballWidth;
	}
	

	BoundingCircle ballCirc = BoundingCircle(ball->position, ballWidth);
#pragma endregion
	GLfloat vertices[] = {
		// Triangle 1		
		-0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		-0.5f, 0.5f, 0.0f,
		//Triangle 2
		-0.5f, 0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		0.5f, 0.5f, 0.0f
	};



	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3* sizeof(GLfloat), (GLvoid*)0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	Shader*	vShader = new Shader((char*)"shaders/vShader.glsl", GL_VERTEX_SHADER);
	Shader*	fShader = new Shader((char*)"shaders/fShader.glsl", GL_FRAGMENT_SHADER);
	ShaderProgram* shader = new ShaderProgram();
	shader->AttachShader(vShader);
	shader->AttachShader(fShader);
	shader->Bind();

	GLint uniform = glGetUniformLocation(shader->GetGLShaderProgram(), (char*)"worldMatrix");
	// This is the game loop, the game logic and render part goes in here.
	// It checks if the created window is still open, and keeps performing the specified operations until the window is closed
	while (!glfwWindowShouldClose(window))
	{

		// Calculate delta time.
		float dt = glfwGetTime();
		// Reset the timer.
		glfwSetTime(0);

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, screenWidth, screenHeight);

		
#pragma region Player_Updates
		player1->position.y += MovePlayer(window, 1) * playerVelocity * dt;
		player1->position.y = glm::clamp(player1->position.y, -0.9f, 0.9f);
		player1->Update();
		

		player2->position.y += MovePlayer(window, 2) * playerVelocity * dt;
		player2->position.y = glm::clamp(player2->position.y, -0.9f, 0.9f);
		player2->Update();
		
#pragma endregion

#pragma region Ball_Updates

		ball->position += ballDirection * ballVelocity * dt;
		if (ball->position.y >= 1 || ball->position.y <= -1)
			ballDirection.y *= -1;

		ball->Update();

		circleCenter.y = ballWidth * 1.5f;
		//Update bounding circles
		for (int i = 0; i < 4; i++)
		{

			p1Circs[i].center.y = player1->position.y + circleCenter.y;
			p2Circs[i].center.y = player2->position.y + circleCenter.y;
			circleCenter.y -= ballWidth;
		}
		ballCirc.center = ball->position;
	
		//Paddle- ball collisions
		if (ball->position.x < 0)
		{
			for (int i = 0; i < 4; i++)
			{
				if (CircleCollision(ballCirc, p1Circs[i]))
				{
					ResolveCollision(player1,ball);
					break;
				}
			}
		}
		else
		{
			for (int i = 0; i < 4; i++)
			{
				if (CircleCollision(ballCirc, p2Circs[i]))
				{
					ResolveCollision(player2, ball);
					break;
				}
			}

		}

		//Respawn ball
		if (ball->position.x >= 1)
		{
			score.x++;
			ball->position = glm::vec3(0.0, -0.8, 0.0);
			ballDirection = glm::normalize(glm::vec3(-1, 1, 0));

			printf("Player 1 Scored! Current score is %u to %u\n", (int)score.x, (int)score.y );
		}
		else if (ball->position.x <= -1)
		{
			score.y++;
			ball->position = glm::vec3(0.0, 0.8, 0.0);
			ballDirection = glm::normalize(glm::vec3(1, -1, 0));
			printf("Player 2 Scored! Current score is %u to %u\n", (int)score.x, (int)score.y);
		}


		ball->Update();

		//Check win condition
		if (score.x == 5)
		{
			printf("Player 1 Wins! Score is reset.\n");
			score.x = score.y = 0;
		}
		if (score.y == 5)
		{
			printf("Player 2 Wins! Score is reset.\n");
			score.x = score.y = 0;
		}
#pragma endregion

		glBindVertexArray(VAO);	

		glUniformMatrix4fv(uniform, 1, GL_FALSE, &ball->GetWorldMatrix()[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glUniformMatrix4fv(uniform, 1, GL_FALSE, &player1->GetWorldMatrix()[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glUniformMatrix4fv(uniform, 1, GL_FALSE, &player2->GetWorldMatrix()[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glBindVertexArray(0);


		// Swaps the front and back buffers of the specified window
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	delete player1, player2, ball, shader;
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	// Terminate all the stuff related to GLFW and exit the program using the return value EXIT_SUCCESS
	glfwTerminate();

	return EXIT_SUCCESS;
}


