#pragma once

// Force GLFW to include gl3.h core functionality instead of gl.h deprecated code.
#define GLFW_INCLUDE_GLCOREARB
#include <GLFW/glfw3.h>

#include <string>
#include <memory>

/*
 * Singleton base class for creating a GLFW window and OpenGL context.
 * Call getInstance() in order to obtain the singleton instance of this class.
 */
class CS488Window {
public:
    virtual ~CS488Window();

	static void launch (
			int argc,
			char **argv,
			CS488Window *window,
			int width,
			int height,
			const std::string& title,
			float fps = 60.0f
	);

protected:
    CS488Window(); // Prevent direct construction.

	static std::string getAssetFilePath(const char *base);

    // Virtual methods.
    // Override these within derived classes.
    virtual void init();
    virtual void appLogic();
	virtual void guiLogic();
    virtual void draw();
    virtual void cleanup();

    // Virtual event handlers.
    // Override these within derived classes.
    virtual bool cursorEnterWindowEvent(int entered);
    virtual bool mouseMoveEvent(double xPos, double yPos);
    virtual bool mouseButtonInputEvent(int button, int actions, int mods);
    virtual bool mouseScrollEvent(double xOffSet, double yOffSet);
    virtual bool windowResizeEvent(int width, int height);
    virtual bool keyInputEvent(int key, int action, int mods);

	GLFWwindow * m_window;
	std::string m_windowTitle;
	int m_windowWidth;
	int m_windowHeight;
	int m_framebufferWidth;
	int m_framebufferHeight;
	bool m_paused;
	bool m_fullScreen;

private:
	static std::shared_ptr<CS488Window> m_instance;

	static std::string m_exec_dir;
    
    GLFWmonitor * m_monitor;

	static std::shared_ptr<CS488Window> getInstance();

	void run (
			int width,
			int height,
			const std::string & windowTitle,
			float desiredFramesPerSecond = 60.0f
	);

	//-- Callback functions to be registered with GLFW:
	static void errorCallback(int error, const char *description);
	static void cursorEnterWindowCallBack(GLFWwindow *window, int entered);
	static void mouseMoveCallBack(GLFWwindow *window, double xPos, double yPos);
	static void mouseButtonCallBack(GLFWwindow * window, int button , int actions, int mods);
	static void mouseScrollCallBack(GLFWwindow * window, double xOffSet, double yOffSet);
	static void windowResizeCallBack(GLFWwindow * window, int width, int height);
	static void keyInputCallBack(GLFWwindow* window, int key, int scancode, int action, int mods);

	void registerGlfwCallBacks();

	void centerWindow();
};
