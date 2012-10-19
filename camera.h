class Camera
{
        float yaw, pitch;
    public:
        float x,y,z;
        void strafe(float);
        void forward(float);
        void up(float);
        void updateYaw(float);
        void updatePitch(float);
        float getYaw();
        float getPitch();
};
