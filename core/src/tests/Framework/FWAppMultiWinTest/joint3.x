xof 0302txt 0064

FWScene fwScene3{
	{grScene3}
	{phScene3}

	FWObject fwBlock1{
		{grFrameBlock1}
		{soBlock1}
	}
	FWObject fwBlock2{
		{grFrameBlock2}
		{soBlock2}
	}
}

GRScene grScene3{
	GRFrame{	#SprGRFrame.h GRFrameDesc
	#Affinef transform
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,1,1;;
		GRFrame grFrameWorld3{	
			1,0,0,0,
			0,1,0,0,
			0,0,1,0,
			0,0,0,1;;
		}
	}

	GRLight light3{
		0.9; 0.9; 0.9; 1.0;;						#Vec4f ambient
		0.5; 0.5; 0.5; 1.9;;						#Vec4f diffuse
		0.1; 0.1; 0.1; 1.0;;						#Vec4f specular
		1.0; 1.0; 1.0; 0.0;;						#Vec4f position
		10000.0;									#float range
		1.0;										#float attenuation0
		0.0;										#float attenuation1
		0.0;										#float attenuation2
		0.0; 0.0; 0.0;;								#Vec3f spotDirection
		0;											#float spotFalloff
		0;											#float spotInner
		0;											#flaot spotCutoff
	}
	
	GRFrame grFrameBlock1{
		#Affinef transform
		1.0, 0.0, 0.0, 0.0,
		0.0, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		0.0, 0.0, 0.0, 1.0;;
		#{cdBox}
	}
	GRFrame grFrameBlock2{
		#Affinef transform
		1.0, 0.0, 0.0, 0.0,
		0.0, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		0.0, 0.0, 0.0, 1.0;;
		#{cdBox}
	}
}

PHScene phScene3{
	#SprPHScene.h PHSceneState (inherited)
		0.01;			#double timeStep
		0				#unsigned count
	;
	0.0; -9.8; 0.0;;	#Vec3f 	gravity
	20;					#int 	numIteratin
	
	PHSolid soBlock1{
		#SprPHSolid.h PHSolidState (inherited)
					0.0; 0.0; 0.0;;		#Vec3d		velocity
			0.0; 0.0; 0.0;;				#Vec3d		angVelocity
			1.0; 0.0; 0.0; 0.0; 0.0; -0.5; 0.0;;	#Posed		pose
			0.0; 0.0; 0.0;;				#Vec3d		force
			0.0; 0.0; 0.0;;				#Vec3d		torque
			0.0; 0.0; 0.0;;				#Vec3d		nextForce
			0.0; 0.0; 0.0;;				#Vec3d		nextTorque
		;
		20.0;						#double		mass
		1.0, 0.0, 0.0,					#Matrix3d	inertia	
		0.0, 1.0, 0.0,
		0.0, 0.0, 1.0;
		0.0; 0.0; 0.0;;					#Vec3d		center
		false;						#bool		dynamical
		CDBox cdBox1{
			#PHMaterialDesc
				0.4		#float mu;				///< 動摩擦摩擦係数
				0.4		#float mu0;				///< 静止摩擦係数
				0.7		#float e;				///< 跳ね返り係数
				1.0		#float density;			///< 密度
			;
			1.5; 1.0; 1.5;;
		}
	}

	PHSolid soBlock2{
		#SprPHSolid.h PHSolidState (inherited)
			0.0; 0.0; 0.0;;				#Vec3d		velocity
			0.0; 0.0; 0.0;;				#Vec3d		angVelocity
			1.0; 0.0; 0.0; 0.0; 0.0; -0.5; 0.0;;	#Posed		pose
			0.0; 0.0; 0.0;;				#Vec3d		force
			0.0; 0.0; 0.0;;				#Vec3d		torque
			0.0; 0.0; 0.0;;				#Vec3d		nextForce
			0.0; 0.0; 0.0;;				#Vec3d		nextTorque
		;
		20.0;						#double		mass
		1.0, 0.0, 0.0,					#Matrix3d	inertia	
		0.0, 1.0, 0.0,
		0.0, 0.0, 1.0;
		0.0; 0.0; 0.0;;					#Vec3d		center
		true;						#bool		dynamical
		CDBox cdBox1{
			#PHMaterialDesc
				0.4		#float mu;		///< 動摩擦摩擦係数
				0.4		#float mu0;		///< 静止摩擦係数
				0.7		#float e;		///< 跳ね返り係数
				1.0		#float density;		///< 密度
			;
			0.5; 3.0; 0.5;;
		}
	}
	
	PHBallJoint joint1{
		
		#SprPHJoint.h PHJointDesc ( == PHConstraintDesc)
			true;	#bool bEnabled
			1.0; 0.0; 0.0; 0.0; 0.0;  0.5; 0.0;;	#Posed poseSocket
			1.0; 0.0; 0.0; 0.0; 0.0; -1.5; 0.0;;	#Posed posePlug
		;
		100			#double		spring;		///< バネ係数
		10			#double		damper;		///< ダンパ係数
		-180; 180;;		#Vec2d		limitSwing;	///< swing角の可動域
		-180; 180;;		#Vec2d		limitTwist;	///< twist角の可動域
		0.0; 0.0; 1.0;;		#Vec3d		limitDir;	///< 可動域の中心ベクトル
		1.0; 0.0; 0.0; 0.0;;	#Quaterniond	goal;		///< バネダンパの制御目標
		0.0; 0.0; 0.0;;		#Vec3d		torque;		///< モータトルク
		600000;			#double		fMax;		///< 関節にかけられる最大の力
		0.0;			#double		fMin;		///< 関節にかけられる最小の力
		{soBlock1}
		{soBlock2}

	}
}