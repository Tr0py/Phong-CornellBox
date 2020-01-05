#version 330 core
out vec4 FragColor;

in vec3 Normal;  
in vec3 FragPos;  
  
uniform vec3 lightPos; 
uniform vec3 viewPos; 
uniform vec3 lightColor;
uniform vec3 objectColor;
uniform vec3 M_ambient;
uniform vec3 M_diffuse;
uniform vec3 M_specular;
//uniform float shininess;


void main()
{
	// BRDF setup
    float ambientStrength = 0.2;
	float diffuseStrength = 0.9;
    float specularStrength = 0.8;
	//vec3 M_ambient = vec3(0.0215,0.1745,0.0215);
	//vec3 M_diffuse = vec3(0.07568,0.61424,0.07568);
	//vec3 M_specular = vec3(0.633,0.727811,0.633);
	float shininess = 0.6;
	shininess *= 128;

    // ambient
    vec3 ambient = ambientStrength * lightColor * M_ambient;

  	
    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), -dot(norm, lightDir));
	diff *= diffuseStrength;
    vec3 diffuse = diff * lightColor * M_diffuse ;

    
    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
	//Phong
    //vec3 reflectDir = reflect(-lightDir, norm);  
    //float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	//Blinn-Phong
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(norm, halfwayDir), -dot(norm, halfwayDir)), shininess);
    vec3 specular = specularStrength * spec * lightColor * M_specular;  
        
    //vec3 result = (ambient + diffuse + specular) * objectColor;
    vec3 result = (ambient + diffuse + specular);
    FragColor = vec4(result, 1.0);
} 
