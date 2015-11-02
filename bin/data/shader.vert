
uniform vec2 offset;
uniform float q, radius;
uniform float zoom;
uniform vec2 window;

const float PI = 3.1415;
const float HC = 180.0/PI;
const float QP = PI/4.0;
const float QC = PI / 360.0;
const float DEG_TO_RAD = PI / 180.0;

float lat2y(float a) {
    return HC * log(tan((QP + a * QC)));
}

vec3 rotate(vec3 _input, float angle, vec3 axis) {
    vec3 ax = normalize(axis);
    float a = float(angle*DEG_TO_RAD);
    float sina = sin( a );
    float cosa = cos( a );
    float cosb = 1.0 - cosa;
    
    float nx = _input.x*(ax.x*ax.x*cosb + cosa)
    + _input.y*(ax.x*ax.y*cosb - ax.z*sina)
    + _input.z*(ax.x*ax.z*cosb + ax.y*sina);
    float ny = _input.x*(ax.y*ax.x*cosb + ax.z*sina)
    + _input.y*(ax.y*ax.y*cosb + cosa)
    + _input.z*(ax.y*ax.z*cosb - ax.x*sina);
    float nz = _input.x*(ax.z*ax.x*cosb - ax.y*sina)
    + _input.y*(ax.z*ax.y*cosb + ax.x*sina)
    + _input.z*(ax.z*ax.z*cosb + cosa);
    
    
    return vec3(nx, ny, nz);
}

void main() {
    vec4 p = gl_Vertex; //ftransform();
    
//    float q = lat2y(p.y);
//    p.y = q;
    
//    float r = 180.0;
    vec3 sphere_point = vec3(0.0, 0.0, radius);
    sphere_point = rotate(sphere_point, p.y, vec3(-1.0, 0.0, 0.0));
    sphere_point= rotate(sphere_point, p.x, vec3(0.0, 1.0, 0.0));
    
    vec3 flat_point = vec3(p.x, lat2y(p.y), 69.0);
    
    vec3 point = mix(flat_point, sphere_point, vec3(q));
    

//    p.x = p.z < 0.0 ? 0.0 : p.x;
    vec4 col = gl_Color;
    vec4 o = gl_ProjectionMatrix * gl_ModelViewMatrix * vec4(point, 1.0);
    if (o.z > window.y - zoom - 180.0 - 69.0) col*= offset.y;

    gl_Position = o;
    gl_FrontColor = col;
}