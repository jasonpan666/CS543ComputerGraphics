#version 150

out vec4 fColor;
in  vec2 texCoord;
uniform sampler2D uImageUnit;
uniform int uColormode;

void main() 
{ 
	if(uColormode == 1){ //Luminance effect
		const vec3 W = vec3(0.2125,0.7154,0.0721);
		vec3 irgb = texture2D(uImageUnit,texCoord).rgb;
		float luminance = dot(irgb,W);
		fColor = vec4(luminance);
	}
	else if(uColormode == 2){ //image negative
		vec3 irgb = texture2D(uImageUnit,texCoord).rgb;
		fColor = vec4(1-irgb.r, 1-irgb.g, 1-irgb.b, 1.0);
	}
	else if(uColormode == 3){ //edge detection effect
		const float T = 0.9;
		vec3 irgb = texture2D(uImageUnit,texCoord).rgb;

		const vec3 LUMCOEFFS = vec3(0.2125,0.7154,0.0721);
		vec2 ires = textureSize(uImageUnit,0);
		float ResS = float(ires.s);
		float ResT = float(ires.t);
		
		vec2 stp0 = vec2(1.0/ResS, 0.0);
		vec2 st0p = vec2(0.0,1.0/ResT);
		vec2 stpp = vec2(1.0/ResS,1.0/ResT);
		vec2 stpm = vec2(1.0/ResS,-1.0/ResT);

		float i00 = dot(texture2D(uImageUnit,texCoord).rgb,LUMCOEFFS);
		float im1m1 = dot(texture2D(uImageUnit,texCoord-stpp).rgb,LUMCOEFFS);
		float ip1p1 = dot(texture2D(uImageUnit,texCoord+stpp).rgb,LUMCOEFFS);
		float im1p1 = dot(texture2D(uImageUnit,texCoord-stpm).rgb,LUMCOEFFS);
		float ip1m1 = dot(texture2D(uImageUnit,texCoord+stpm).rgb,LUMCOEFFS);
		float im10 = dot(texture2D(uImageUnit,texCoord-stp0).rgb,LUMCOEFFS);
		float ip10 = dot(texture2D(uImageUnit,texCoord+stp0).rgb,LUMCOEFFS);
		float i0m1 = dot(texture2D(uImageUnit,texCoord-st0p).rgb,LUMCOEFFS);
		float i0p1 = dot(texture2D(uImageUnit,texCoord+st0p).rgb,LUMCOEFFS);
		float h = -1.0*im1p1-2.0*i0p1-1.0*ip1p1+1.0*im1m1+2.0*i0m1+1.0*ip1m1;
		float v = -1.0*im1m1-2.0*im10-1.0*im1p1+1.0*ip1m1+2.0*ip10+1.0*ip1p1;

		float mag = sqrt(h*h + v*v);
		vec3 target = vec3(mag,mag,mag);
		fColor = vec4(mix(irgb,target,T),1.0);
	}
	else if(uColormode == 4){ //embossing effect
		const float T = 0.1;
		vec3  irgb = texture2D(uImageUnit,texCoord).rgb;

		const vec3 LUMCOEFFS = vec3(0.2125,0.7154,0.0721);
		vec2  ires = textureSize(uImageUnit,0);
		float ResS = float(ires.s);
		float ResT = float(ires.t);
		
		vec2 stp0 = vec2(1.0/ResS, 0.0);
		vec2 stpp = vec2(1.0/ResS,1.0/ResT);

		vec3 c00 = texture2D(uImageUnit,texCoord).rgb;
		vec3 cp1p1 = texture2D(uImageUnit,texCoord+stpp).rgb;

		vec3 diffs = c00 - cp1p1;
		float max = diffs.r;
		if(abs(diffs.g)>abs(max))
			max = diffs.g;
		if(abs(diffs.b)>abs(max))
			max = diffs.b;

		float gray = clamp(max+0.5,0.0,1.0);
		vec4 grayVersion = vec4(gray,gray,gray,1.0);
		vec4 colorVersion = vec4(gray*c00,1.0);
		fColor = mix(grayVersion, colorVersion, T);
		//fColor = vec4(mix(irgb,vec3(0.5,0.5,0.5),0.2),1.0);
		//fColor = texture2D(uImageUnit,texCoord);
	}
	else if(uColormode == 5){ //Toon rendering
		const float threshold = 0.15;
		const float uQuantize = 15;
		vec3  irgb = texture2D(uImageUnit,texCoord).rgb;

		const vec3 LUMCOEFFS = vec3(0.2125,0.7154,0.0721);
		vec2  ires = textureSize(uImageUnit,0);
		float ResS = float(ires.s);
		float ResT = float(ires.t);
		
		vec2 stp0 = vec2(1.0/ResS, 0.0);
		vec2 st0p = vec2(0.0,1.0/ResT);
		vec2 stpp = vec2(1.0/ResS,1.0/ResT);
		vec2 stpm = vec2(1.0/ResS,-1.0/ResT);

		float i00 = dot(texture2D(uImageUnit,texCoord).rgb,LUMCOEFFS);
		float im1m1 = dot(texture2D(uImageUnit,texCoord-stpp).rgb,LUMCOEFFS);
		float ip1p1 = dot(texture2D(uImageUnit,texCoord+stpp).rgb,LUMCOEFFS);
		float im1p1 = dot(texture2D(uImageUnit,texCoord-stpm).rgb,LUMCOEFFS);
		float ip1m1 = dot(texture2D(uImageUnit,texCoord+stpm).rgb,LUMCOEFFS);
		float im10 = dot(texture2D(uImageUnit,texCoord-stp0).rgb,LUMCOEFFS);
		float ip10 = dot(texture2D(uImageUnit,texCoord+stp0).rgb,LUMCOEFFS);
		float i0m1 = dot(texture2D(uImageUnit,texCoord-st0p).rgb,LUMCOEFFS);
		float i0p1 = dot(texture2D(uImageUnit,texCoord+st0p).rgb,LUMCOEFFS);
		float h = -1.0*im1p1-2.0*i0p1-1.0*ip1p1+1.0*im1m1+2.0*i0m1+1.0*ip1m1;
		float v = -1.0*im1m1-2.0*im10-1.0*im1p1+1.0*ip1m1+2.0*ip10+1.0*ip1p1;

		float mag = length(vec2(h,v));
		if(mag>threshold)
			fColor = vec4(0.0,0.0,0.0,1.0);
		else{
			irgb.rgb *= uQuantize;
			irgb.rgb += vec3(0.5,0.5,0.5);
			vec3 intrgb = vec3(irgb.rgb);
			irgb.rgb = vec3(intrgb)/uQuantize;
			fColor = vec4(irgb,1.0);
		}
	}
	else if(uColormode == 6){ //Twirl Transformation
		const float PI = 3.14159265;
		const float uD = 45; //angle
		const float uR = 0.8; //change area 0-1

		vec2  ires = textureSize(uImageUnit,0);
		float ResS = float(ires.s);
		float ResT = float(ires.t);
		
		vec2 st = texCoord;
		float Radius = ResS * uR;
		vec2 xy = ResS * st;

		vec2 dxy = xy - ResS/2.0;
		float r = length(dxy);
		float beta = atan(dxy.y, dxy.x) + radians(uD) * (Radius - r)/Radius;

		vec2 xy1 = xy;
		if(r<=Radius){
			xy1 = ResS/2.0 + r * vec2(cos(beta), sin(beta));
		}
		st = xy1/ResS;

		vec3  irgb = texture2D(uImageUnit,st).rgb;
		fColor = vec4(irgb,1.0);
	}
	else if(uColormode == 7){ // Ripple Transformation
		const float PI = 3.14159265;
		const float ax = 10;
		const float ay = 15;
		const float tx = 150;
		const float ty = 250;

		vec2  ires = textureSize(uImageUnit,0);
		float ResS = float(ires.s);
		float ResT = float(ires.t);
		
		vec2 st = texCoord;
		vec2 xy = ResS * st; //x' y'

		vec2 xy1;
		xy1.x = xy.x + ax*sin(2*PI*xy.y/tx);
		xy1.y = xy.y + ay*sin(2*PI*xy.x/ty);
		st = xy1/ResS;

		vec3  irgb = texture2D(uImageUnit,st).rgb;
		fColor = vec4(irgb,1.0);
	}
	else if(uColormode == 8){ // Spherical Transformation
		const float PI = 3.14159265;
		const float uR = 0.8; //change area 0-1
		const float p = 1.8;

		vec2  ires = textureSize(uImageUnit,0);
		float ResS = float(ires.s);
		float ResT = float(ires.t);
		
		vec2 st = texCoord;
		float Radius = ResS * uR / 2; //rmax half image width
		vec2 xy = ResS * st;  //x' y'

		vec2 dxy = xy - ResS/2.0;//dx dy
		float r = length(dxy);  // _|dx^2+dy^2  r
		float z = sqrt(Radius * Radius - r * r);  //z

		float betax = (1-1/p)* asin(dxy.x/sqrt(dxy.x*dxy.x+z*z));
		float betay = (1-1/p)* asin(dxy.y/sqrt(dxy.y*dxy.y+z*z));

		vec2 xy1 = xy - vec2(0,0);
		if(r<=Radius){
			xy1.x = xy.x - z*tan(betax);
			xy1.y = xy.y - z*tan(betay);
		}
		st = xy1/ResS;

		vec3  irgb = texture2D(uImageUnit,st).rgb;
		fColor = vec4(irgb,1.0);
	}
	else{
		fColor = texture2D(uImageUnit,texCoord);
	}
} 

