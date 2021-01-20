#include "rtweekend.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

#include "opengl_files/ShaderC.h"
#include "imguiWrapper.h"

#include "camera.h"
#include "color.h"
#include "hittable_list.h"
#include "sphere.h"
#include "material.h"

#include "stb_image.h"
#include "stb_image_write.h"

#include <thread>


color ray_color(const ray& r ,const hittable& world,int depth) {
	hit_record rec;

	// if we've exceded the bounce limit no light is produced
	if (depth <= 0)
		return color(0, 0, 0);

	if (world.hit(r, 0.0001, infinity, rec)) {
        ray scattered;
        color attenuation;
        // -- scatter only returns false on degenerate return rays ? 
        if (rec.mat_ptr->scatter(r, rec, attenuation, scattered))
            return attenuation * ray_color(scattered, world, depth - 1);
        return color(0, 0, 0);
		
	}

	// -- void color. recursion returns from here if it doesnt hit anyting
    // -- then this return color * attanuation1 * attenuation2 * ...
	vec3 unit_direction = unit_vector(r.direction());
	auto t = 0.5 * (unit_direction.y() + 1.0);
	return (1.0 - t) * color(1.0, 1.0, 1.0) + t * color(0.5, 0.7, 1.0);
}

//multithreading
void bindTexture(std::string path);
volatile bool needto_bind_texture = false;
volatile int remaining_render_rows = 1;

// image
const auto aspect_ratio = 16.0 / 9.0;
int image_width = 400;
int image_height =(int)(image_width / aspect_ratio);
int samples_per_pixel = 100;
int depth = 50;
//effects the number of the random spheres
int random_sphere_val = 0;

// cam vars
double vfov = 45.0;
point lookfrom(13, 2, 3);
point lookat(0, 0, 0);
vec3 vup(0, 1, 0);
auto dist_to_focus = 10.0;
double aperture = 0.1;
hittable_list random_scene()
{
    hittable_list world;

    auto ground_material = make_shared<lambertian>(color(0.5, 0.5, 0.5));
    world.add(make_shared<sphere>(point(0, -1000, 0), 1000, ground_material));

    for (int a = -random_sphere_val /2; a < random_sphere_val / 2; a++) {
        for (int b = -random_sphere_val / 2; b < random_sphere_val / 2; b++) {
            auto choose_mat = random_double();
            point center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());

            if ((center - point(4, 0.2, 0)).length() > 2.9*(1.0/(double)random_sphere_val)) {
                shared_ptr<material> sphere_material;

                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = color::random() * color::random();
                    sphere_material = make_shared<lambertian>(albedo);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                }
                else if (choose_mat < 0.95) {
                    // metal
                    auto albedo = color::random(0.5, 1);
                    auto fuzz = random_double(0, 0.5);
                    sphere_material = make_shared<metal>(albedo, fuzz);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                }
                else {
                    // glass
                    sphere_material = make_shared<dielectric>(1.5);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                }
            }
        }
    }

    auto material1 = make_shared<dielectric>(1.5);
    world.add(make_shared<sphere>(point(0, 1, 0), 1.0, material1));

    auto material2 = make_shared<lambertian>(color(0.4, 0.2, 0.1));
    world.add(make_shared<sphere>(point(-4, 1, 0), 1.0, material2));

    auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
    world.add(make_shared<sphere>(point(4, 1, 0), 1.0, material3));
    return world;
}
void render_image()
{

    hittable_list world = random_scene();
    camera cam(lookfrom, lookat, vup, vfov, aspect_ratio,aperture,dist_to_focus);

	// stb_image_write code
	const int channels = 3;
	uint8_t* pixels = new uint8_t[(int)image_width * (int)image_height * (int)channels];
	int index = 0;

	//render
	std::cout << "\n" << image_width << ' ' << image_height << "\n";
	//-reverse for because we want rows top to bottom-
	for (int j = image_height - 1; j >= 0; j--)
	{
		//-progress indicator-
		std::cerr << "\rkalan pixel satiri: " << j << ' ' << std::flush;
        remaining_render_rows = j;
		for (int i = 0; i < image_width; i++)
		{
			color pixel_color(0, 0, 0);
			for (size_t s = 0; s < samples_per_pixel; s++)
			{
				//-0->255 to 0->1.0
				auto u = (i + random_double()) / (image_width - 1);
				auto v = (j + random_double()) / (image_height - 1);
				ray r = cam.get_ray(u, v);
				pixel_color += ray_color(r, world, depth);
			}
			//write_color(std::cout, pixel_color,samples_per_pixel);
			write_color_stb(index, pixels, pixel_color, samples_per_pixel);
		}
	}
	stbi_write_jpg("image.jpg", image_width, image_height, 3, pixels, 100);
	delete[] pixels;
	std::cerr << "\nDone\n";
    needto_bind_texture = true;
}

// OPENGL
unsigned int window_width = 1280;
unsigned int window_height = 720;
float iTime = 3;
void bindTexture(std::string path)
{
    // load and create a texture 
    // -------------------------
    unsigned int texture1;
    // texture 1
    // ---------
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture1);

}
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}
GLFWwindow* initOpengl(float quadWith,float quadHeight)
{
    // glfw: initialize and configure
   // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(window_width, window_height, "Adem macit Tez projesi", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
    }

  

    float vertices[] = {
        // first triangle
		quadWith,  quadHeight, 0.0f, 1.0f,1.0f, // top right
		quadWith , 0.0f, 0.0f,   1.0f,0.0f,// bottom right
	     0.0f, quadHeight, 0.0f,  0.0f,1.0f, // top left 
        // second triangle
		quadWith,0.0f, 0.0f,  1.0f,0.0f, // bottom right
		0.0f,0.0f, 0.0f,  0.0f,0.0f, // bottom left
		0.0f, quadHeight, 0.0f  , 0.0f,1.0f // top left
    };
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);


    bindTexture("image.jpg");

    
    return window;
}

//IMGUI
bool show_cpu = true;
bool show_gpu = false;
bool render_changed_flag = false;
void drawUI()
{
    static float debug_interval = 0.1f;

    ImGui::Begin("kontrol paneli");                         
    ImGui::Text("frame (%.1f FPS)", ImGui::GetIO().Framerate);
    

    if (ImGui::RadioButton("CPU", show_cpu)) {
        if (show_cpu == false)
            render_changed_flag = true;
        show_cpu = true;
        show_gpu = false;
    }
    ImGui::SameLine();
    if(ImGui::RadioButton("GPU", show_gpu)) {
        if (show_gpu == false)
            render_changed_flag = true;
        show_cpu = false;
        show_gpu = true;
    }

    if(show_cpu)
    {
        ImGui::PushItemWidth(100);
        int tempWidth = image_width;
        if (ImGui::InputInt("width(pixel sayisi)", &tempWidth))
        {
            image_width = tempWidth;
            image_height = static_cast<int>(image_width / aspect_ratio);
        }
        ImGui::InputInt("pixel basina ornek sayisi", &samples_per_pixel);
        ImGui::InputInt("max yansima sayisi(depth)", &depth);
        ImGui::InputDouble("kamera acisi(fov)", &vfov);
        ImGui::InputDouble("lens genisligi(aperture)", &aperture);
        static int combo_secili = 0;
        if (ImGui::Combo("kucuk kurelerin yogunlugu", &combo_secili, "yok\0az\0orta\0cok\0\0"))
        {
            if (combo_secili == 0)
                random_sphere_val = 0;
            else if(combo_secili == 1)
                random_sphere_val = 8;
            else if (combo_secili == 2)
                random_sphere_val = 22;
            else if (combo_secili == 3)
                random_sphere_val = 38;
        }
        ImGui::PopItemWidth();

        if (ImGui::Button("render"))
        {
            std::thread render_thread(render_image);
            render_thread.detach();
        }
        float remainder_fraction =  1.0f - (float)remaining_render_rows / (float)image_height;
        ImGui::ProgressBar(remainder_fraction);
    }
    else
    {
        ImGui::SliderFloat("kamera degiskeni(iTime) ", &iTime, 0.0f, 10.0f);
    }

    ImGui::End();
}

int main()
{
	GLFWwindow* window = initOpengl(image_width, image_height);
    ImguiWrapper::init(window);

    // GPU RENDER
    Shader pixel_shader("opengl_files/pixel_shaders/pixelVertex.glsl", "opengl_files/pixel_shaders/pixelFragment.glsl");
    glm::mat4 orthoProjMat = glm::ortho(0.0f, (float)image_width, 0.0f, (float)image_height);
    pixel_shader.setMat4("projection", orthoProjMat);
    pixel_shader.setInt("texture0", 0);
    pixel_shader.setFloat("iTime", 12.0f);
    pixel_shader.setVec2("iResolution", { window_width,window_height });

    // CPU RENDER
    Shader ortho_shader("opengl_files/default_shaders/vertexOrtho.glsl", "opengl_files/default_shaders/fragmentOrtho.glsl");
    ortho_shader.setMat4("projection", orthoProjMat);
    ortho_shader.setInt("texture0", 0);


    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImguiWrapper::frameStart();

        if (needto_bind_texture)
        {
            bindTexture("image.jpg");
            needto_bind_texture = false;
        }

        if (render_changed_flag&&show_cpu)
        {
            render_changed_flag = false;
            ortho_shader.use();
        }
        else if(render_changed_flag && show_gpu)
        {
            render_changed_flag = false;
            pixel_shader.use();
        }

        pixel_shader.setFloat("iTime", iTime);

        glDrawArrays(GL_TRIANGLES, 0, 6);


        drawUI();

        ImguiWrapper::frameEnd();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }


    ImguiWrapper::terminate();
    glfwTerminate();

}

