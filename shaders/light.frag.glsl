# version 120 

/* This is the fragment shader for reading in a scene description, including 
   lighting.  Uniform lights are specified from the main program, and used in 
   the shader.  As well as the material parameters of the object.  */

// Mine is an old machine.  For version 130 or higher, do 
// in vec4 color ;  
// in vec4 mynormal ; 
// in vec4 myvertex ;
// That is certainly more modern

varying vec4 color ;
varying vec3 mynormal ; 
varying vec4 myvertex ; 
varying vec4 mypos;				
const int numLights = 10 ; 
uniform bool enablelighting ; // are we lighting at all (global).
uniform vec4 lightposn[numLights] ; // positions of lights 
uniform vec4 lightcolor[numLights] ; // colors of lights
uniform float attenuate[numLights];
uniform int numused ;               // number of lights used

// Now, set the material parameters.  These could be varying and/or bound to 
// a buffer.  But for now, I'll just make them uniform.  
// I use ambient, diffuse, specular, shininess as in OpenGL.  
// But, the ambient is just additive and doesn't multiply the lights.  

uniform vec4 ambient ; 
uniform vec4 diffuse ; 
uniform vec4 specular ; 
uniform vec4 emission ; 
uniform float shininess ; 

uniform sampler2D tex0;
uniform bool enableTex;

vec4 ComputeLight (const in vec3 direction, const in vec4 lightcolor, const in vec3 normal, const in vec3 halfvec, const in vec4 mydiffuse, const in vec4 myspecular, const in float myshininess) {

        float nDotL = dot(normal, direction)  ;         
        vec4 lambert = mydiffuse * lightcolor * max (nDotL, 0.0) ;  

        float nDotH = dot(normal, halfvec) ; 
        vec4 phong = myspecular * lightcolor * pow (max(nDotH, 0.0), myshininess) ; 

        vec4 retval = lambert + phong ; 
        return retval ;            
}       

void main (void) 
{       
    if (enablelighting) {       
        
        vec4 finalcolor = vec4(0,0,0,0); 

        // YOUR CODE FOR HW 2 HERE
        // A key part is implementation of the fragment shader
	const vec3 eyepos = vec3(0,0,0);
	vec4 _mypos = gl_ModelViewMatrix * myvertex;
	vec3 mypos = _mypos.xyz / _mypos.w;
	vec3 eyedirn = normalize(eyepos - mypos);

	vec3 _normal = (gl_ModelViewMatrixInverseTranspose*vec4(mynormal,0.0)).xyz;
	vec3 normal = normalize(_normal);

	finalcolor = ambient + emission;
	vec3 direction;
	for (int i=0; i<numused; i++) {    	    
	    if (lightposn[i].w == 0) {
	        direction = normalize(lightposn[i].xyz);
	    }
	    else {
                vec3 position = lightposn[i].xyz / lightposn[i].w;
	        direction = normalize(position - mypos);
	    }
	    vec3 half0 = normalize(direction + eyedirn);
	    vec4 col = ComputeLight(direction, lightcolor[i], normal, half0, diffuse, specular, shininess);
	    if(lightposn[i].w == 0)
	        col = col;
	    if(attenuate[i] > 0)
	        finalcolor += col * 1/(sqrt((distance(mypos.xyz, lightposn[i].xyz))*attenuate[i]));
	    else
	        finalcolor += col;
	    }          
	if (enableTex)
            gl_FragColor = finalcolor * texture2D(tex0, gl_TexCoord[0].st); 
	else
	    gl_FragColor = finalcolor;
	}

    else
        if (enableTex) 
            gl_FragColor = texture2D(tex0, gl_TexCoord[0].st);
        else
            gl_FragColor = ambient + emission;
}
