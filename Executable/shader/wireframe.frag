#version 460

uniform struct LineInfo{
	float Width;
	vec4 Color;
}Line;


in vec3 GPosition;
in vec3 GNormal;
noperspective in vec3 GEdgeDistance;

layout(location = 0) out vec4 FragColor;

void main()
{
	vec4 color = vec4(0.0);
	
	float d = min(GEdgeDistance.x,GEdgeDistance.y);
	d = min(d,GEdgeDistance.z);

	float mixVal;
	if(d < Line.Width - 1)
	{
		mixVal = 1.0;
	}
	else if(d > Line.Width + 1)
	{
		mixVal = 0.0;
	}
	else
	{
		float x = d- (Line.Width-1);
		mixVal = exp2(-2 * (x*x));
	}

	
	FragColor = mix(color,Line.Color,mixVal);

	if (FragColor.a < 0.01)
    discard; 
}