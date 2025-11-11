#version 330 core
out vec4 FragColor;

float near = 0.1; 
float far = 100.0; 
//屏幕空间中的深度值是非线性的，即它在z值很小的时候有很高的精度，
//而z值很大的时候有较低的精度。片段的深度值会随着距离迅速增加，
//所以几乎所有的顶点的深度值都是接近于1.0的
//我们可以反转这个非线性方程，应用到最终的深度值上
float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    //逆变换获取线性深度值
    return (2.0 * near * far) / (far + near - z * (far - near));	
}

void main()
{
    float depth = LinearizeDepth(gl_FragCoord.z) / far; // divide by far to get depth in range [0,1] for visualization purposes
    FragColor = vec4(vec3(depth), 1.0);
}
