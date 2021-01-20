#version 330 core
uniform float iTime;
uniform vec2 iResolution;
out vec4 FragColor;

struct ray {
	vec3 origin;
    vec3 direction;
};
    
struct hit_record {
	float t;
    vec3 p;
    vec3 normal;
    bool hit;
    vec3 color;
    int material;
};
    
struct sphere {
	vec3 center;
    float radius;
    vec3 color;
    int material;
};
    
vec3 walk(ray r, float t) {
	return r.origin + normalize(r.direction) * t;
}

hit_record hit_sphere(sphere s, const ray r, float t_max, float t_min) {
    vec3 oc = r.origin - s.center;
    float a = dot(r.direction, r.direction);
    float b = dot(oc, r.direction);
    float c = dot(oc, oc) - s.radius*s.radius;
    float discriminant = b*b - a*c;
   
	if (discriminant > 0.) {
        float temp = (-b - sqrt(discriminant)) / a;
        if (temp < t_max && temp > t_min) {
            hit_record rec = hit_record(
                temp,
                walk(r, temp),
                (walk(r, temp) - s.center) / s.radius,
                true,
                s.color,
                s.material
            );
       
            return rec;
        }
        
        temp = (-b + sqrt(discriminant)) / a;
        if (temp < t_max && temp > t_min) {
            hit_record rec = hit_record(
            temp,
            walk(r, temp),
            (walk(r, temp) - s.center) / s.radius,
            true,
                s.color,
                s.material
            );
          
            return rec;
        }
    }

    return hit_record(
            0.0,
            vec3(0.0,0.0,0.0),
            vec3(0.0,0.0,0.0),
            false,
        	vec3(0.0,0.0,0.0),
        	0
           );
}

float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

vec3 bounce(ray r, inout sphere[3] spheres) {
    for(int i = 0; i < spheres.length(); i++) {
        hit_record t = hit_sphere(spheres[i], r, 100.0, 0.0);
        if (t.hit == true) {
            vec3 N = normalize(t.normal);
            return t.color;
        }
    }
    
 	vec3 unit_direction = normalize(r.direction);
    float t = 2.0*(unit_direction.y);
    return (1.0-t)*vec3(1.0, 1.0, 1.0) + t*vec3(0.5, 0.7, 1.0);
}

vec3 random_in_unit_sphere(vec3 p) {
  return normalize(vec3(rand(p.xy * iTime), rand(p.yz / iTime), rand(p.zy + iTime)));
}


vec3 color(ray r, inout sphere[3] spheres) {
    for(int i = 0; i < spheres.length(); i++) {
        hit_record t = hit_sphere(spheres[i], r, 100.0, 0.0);
        if (t.hit == true) {
            vec3 N = normalize(t.normal);
            if (t.material == 1) {
                vec3 reflected = reflect(normalize(t.p), t.normal);
                ray scattered = ray(t.p, reflected);
            	return .5 * t.color + .5 * bounce(scattered,spheres);
            } else {
                vec3 target = t.p + t.normal + random_in_unit_sphere(t.p);
                ray scattered = ray(t.p, target - t.p);
            	return t.color + .5 * bounce(scattered,spheres);
            }
        }
    }
    vec3 unit_direction = normalize(r.direction);
    float t = 2.0*(unit_direction.y);
    return (1.0-t)*vec3(1.0, 1.0, 1.0) + t*vec3(0.5, 0.7, 1.0); 
}



void main(){
    // Normalized pixel coordinates (from 0 to 1)
    vec2 uv = gl_FragCoord.xy/iResolution.xy;
	uv -= .5;
    uv.x *= iResolution.x/iResolution.y;
    
    vec3 horizontal = 	vec3(1.0, 0.0, 0.0);
    vec3 vertical 	= 	vec3(0.0, 1.0, 0.0);
    vec3 origin 	=	vec3(.2 + cos(iTime * 2.) * .05, sin(iTime) * .01,-.1 - sin(iTime) * .1);
    vec3 center		=	vec3(0.0, 0.0, -1.0);
    
    
    // X axis
    if (uv.y < 0.005 && uv.y > -0.005) {
    	FragColor = vec4(1.0,0.0,0.0,1.0);
        //return;
    }
    
    // Y axis
    if (uv.x < 0.005 && uv.x > -0.005) {
    	FragColor = vec4(0.0,1.0,0.0,1.0);
        //return;
    }
    
    sphere[3] spheres = sphere[3](
        sphere(vec3(0.0,0.0,-1.), .25, vec3(.2, 0.1, 0.1), 0),
        sphere(vec3(.5 + cos(iTime) * .2, -0.15, - 1.2 - sin(iTime) *.1), .10, vec3(.0, 0.0, 0.5), 1),
        sphere(vec3(0.0,-100.25,-1.), 100.00, vec3(0.2, 0.5, 0.2), 0)
    );
    
    vec3 result = vec3(0.0, 0.0, 0.0);

    int N = 10;
    
    for (int i = 0; i < N; i++) {
        ray r = ray(origin, 
                    center + 
                    uv.x * horizontal 
                    + uv.y * vertical
                    //Antialiasing
                    + vec3((cos(float(i)/float(N) * 3.1415) * 0.001), (sin(float(i)/float(N) * 3.1415) * 0.001),0.0)
                   );
    	result += color(r, spheres);
    }
    result /= float(N);
    
    FragColor = vec4(result, 1.0);  
}