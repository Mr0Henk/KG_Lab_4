// подключение всех библиотек

//  ********    !!!ВООБЩЕ НЕ ТРОГАТЬ!!!   *********
#include <GLFW/glfw3.h>
#include <imgui.h>
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include <iostream>
#include <vector>
#include <cmath> // +математика
//  ********    !!!ВООБЩЕ НЕ ТРОГАТЬ!!!   *********


bool drawMode = true; // рисование вкл/выкл

//структура точки
struct Point {
    float x, y;
    Point(float _x, float _y) : x(_x), y(_y) {}
};

//массив вектор таких точек
std::vector<Point> Points;


bool isDragging = false;
int draggedPointIndex = -1;

// рисуем точку
void drawPoint(float x, float y) {
    glColor3f(0.3f, 0.8f, 0.3f);                                           //цвет точек
    glPointSize(8);
    glBegin(GL_POINTS);
    glVertex2f(x, y);
    glEnd();
}

//факториал
int factorial(int n) {
    int result = 1;
    for(int i = 1; i <= n; i++) {
        result *= i;
    }
    return result;
}

//полином
float polynom(int i, int n, float t) {
    return (factorial(n)/(factorial(i) *
                          factorial(n - i)))* (float)pow(t, i) *
           (float)pow(1 - t, n - i);
}

//рисовалка кривых
void drawBezierCurve() {
    glLineWidth(2);
    glColor3f(0.1f, 1.0f, 0.1f);                                        //цвет кривых

    int n = Points.size() - 1;
    if (n < 1) return;
    glBegin(GL_LINE_STRIP);

    int j = 0;
    float step = 0.01;
    float t = 0;
    while (t < 1) {
        float ytmp = 0;
        float xtmp = 0;
        for (size_t i = 0; i < Points.size(); i++) {
            float b = polynom(i, Points.size() - 1, t);
            xtmp += Points[i].x * b;
            ytmp += Points[i].y * b;
        }
        glVertex2f(xtmp, ytmp);
        j++;
        t += step;
    }
    glEnd();
}


//попытка в интерфейс имгуи
void renderInterface() {
    ImGui::Begin("warning information");
    ImGui::End();
}


//нажатие мыши
void mouseClick(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS) {
        drawMode = !drawMode;
    }
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE and drawMode == true) {
        if (!isDragging) {
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);
            int width, height;
            glfwGetWindowSize(window, &width, &height);
            Points.push_back(Point((float) xpos, (float) (height - ypos)));
            glfwPostEmptyEvent();
        } else {
            isDragging = false;
        }
    }
    else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
        // Удаление последней точки при нажатии правой кнопки мыши
        if (!Points.empty()) {
            Points.pop_back();
            // Дополнительные действия при удалении точки, если нужно
        }
    }
}

// движение мыши
void mouseMotion(GLFWwindow* window, double xpos, double ypos) {
    if (isDragging && draggedPointIndex >= 0 && draggedPointIndex < Points.size()) {
        int width, height;
        glfwGetWindowSize(window, &width, &height);
        Points[draggedPointIndex].x = xpos;
        Points[draggedPointIndex].y = height - ypos;
        glfwPostEmptyEvent();
    }
}

// реакция клавиатуры
//выход по кнопке ESC
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) // функция выхода от кнопки ESC
{
    // Когда пользователь нажимает ESC, мы устанавливаем свойство WindowShouldClose в true,
    // и приложение после этого закроется
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}

//отработка дисплея
void display(GLFWwindow* window) {
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(0.0f, 0.0f, 0.0f);

    for (int i = 0; i < Points.size(); ++i) {
        drawPoint(Points[i].x, Points[i].y);
    }
    if (Points.size() >= 2) {
        drawBezierCurve();
    }

    glfwSwapBuffers(window);
}

//переделка формы
void reshape(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width, 0, height, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

// нужно для
void mouseDrag(GLFWwindow* window, double xpos, double ypos) {
    if (!isDragging) {
        for (int i = 0; i < Points.size(); ++i) {
            if (abs(Points[i].x - xpos) <= 5 && abs(Points[i].y - ypos) <= 5) {
                isDragging = true;
                draggedPointIndex = i;
                break;
            }
        }
    }
}

//*****************************************************************
//**  important   !!! кнопки управления теже   !!!    important  **
//**  information !!!  что и в прошлой лабе    !!!   information **
//*****************************************************************

int main() {

    if (!glfwInit()) {
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(1024, 768, "Curved Line", NULL, NULL);
    if (window == NULL) {
        std::cout << "Oh no! GLFW is De@d!" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    // ImGui в проге не появляться, но без него ни как


    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    glfwSetMouseButtonCallback(window, mouseClick); // ловец мышки
    glfwSetCursorPosCallback(window, mouseMotion);  //ловец кнопок мышки
    glfwSetKeyCallback(window, key_callback);       //ловец клавы


    while (!glfwWindowShouldClose(window)) { // основной цыкл
    glClear(GL_COLOR_BUFFER_BIT);

        // не работаетттттт\\
    //Start a new ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Text("Hello, ImGui!");
    if (ImGui::Button("Press Me"))
    {
            ImGui::Text("Hello, ImGui!");
    }
        // не работаетттттт\\



    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width, 0, height, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    // цвет фона
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);



    //  ********    это база! не трогать!   *********
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    display(window);

    glfwPollEvents();
}
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
    //  ********    это база! не трогать!   *********

    return 0;
}
