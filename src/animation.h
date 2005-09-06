#ifndef DEF_ANIMATION_H
#define DEF_ANIMATION_H 1

class EmptyAnimation {
    public:
        EmptyAnimation(EmptyAnimationPtr* afallback=NULL);
        virtual ~EmptyAnimation() { }
        virtual void setBasePos(SDL_Rect*);
        virtual void unsetBasePos();
        virtual void runAnim();
        virtual bool updateAnim(Uint16);
        virtual Frame getFrame() const;
        virtual SDL_Rect getDrawPos() const;
        virtual SDL_Rect getFrameDim() const;
        virtual Frame getBaseFrame() const;
        virtual bool isValid() const;
        virtual bool isRunning() const;
        virtual bool isImage() const;
        virtual bool isFixed() const;
        virtual void setFallBack(EmptyAnimationPtr* newfallback=NULL);
    protected:
        Frame base_frame;
    private:
        EmptyAnimationPtr* fallback;
};

/** \brief Animation is responsible to return the correct frame of an animation

*/
class Animation : public EmptyAnimation {
    public:
        /// \brief Initialize the animation
        /// \param abase_image Base image used for the animation
        /// \param aframes Number of frames
        /// \param abp_type Base point type
        /// \param aanimation_type Animation type
        /// \param afps FPS of the animation, if (afps < 0) it is considered to be the duration in ms, if (afps==0) a default duration DATA_LVLDUR is taken
        /// \param astart_pos Start position from the frame array of the base_image
        /// \param aallign_type Allignment type
        ///
        /// To load one still image simply use: Animation(imgcache->loadImage(1,"image_name.png"))
        Animation(const Image& abase_image,
                  Uint16 aframes=1,
                  BasePointType abp_type=BP_MD,
                  Uint16 aanimation_type=ATYPE_LOOP,
                  double afps=0,
                  Uint16 astart_pos=0,
                  AllignType aallign_type=AT_MD);
        Animation();
        virtual ~Animation();
        /// Set the base position of the animation
        virtual void setBasePos(SDL_Rect* abase_pos) {
            base_pos=abase_pos;
            checkAnim();
        }
        /// Unsets the base position of the animation
        virtual void unsetBasePos() {
            base_pos=NULL;
        }
        /// Start the animation from the beginning (used when the animation was still)
        virtual void runAnim();
        /// Updates a running animation and stops it if necessary
        /// return True if the animation is still running
        virtual bool updateAnim(Uint16 dt);
        /// Calculates and returns the current frame
        virtual Frame getFrame() const;
        /// Calculates and returns the current draw position using:
        /// base_pos,bp_type,allign_type
        virtual SDL_Rect getDrawPos() const;
        /// HACK: Return the dimensions of the first frame
        virtual SDL_Rect getFrameDim() const;
        /// HACK: Return the base frame
        virtual Frame getBaseFrame() const;
        /// return True if the animation is valid
        virtual bool isValid() const {
            return is_valid;
        }
        /// return True if the animation is running
        virtual bool isRunning() const {
            return is_running;
        }   
        /// return True if the animation is a still image
        virtual bool isImage() const {
            return is_image;
        }   
        /// return True if the animation dimensions stay the same
        virtual bool isFixed() const {
            return is_fixed;
        }
        virtual void setFallBack(EmptyAnimationPtr*) { }
    protected:
        /// Helper function to set the shift values of the animation
        void setShift();
        /// Helper function to stop an animation
        bool stopAnim();
        /// check validity of the animation
        bool checkAnim();
    protected:
        /// Base image for the animation (big)
        Image base_image;
        /// Number of frames
        Uint16 frames;
        ///\brief The base point of the animation
        ///
        /// The base point always stays fixed during the animation.
        /// It's types are: BP_xy, where x may be either L,M or R and y may be either
        /// U, M or D. L: left end, M: middle, R: right end, U: upper end, D: lower end
        ///
        /// Example: BP_MU means the BP is the upper middle point of each frame.
        /// Default: BP_MD
        BasePointType bp_type;
        /// Animation type: ATYPE_ONCE (play once), ATYPE_LOOP (always play, jump back to
        /// the beginning), ATYPE_SWING (always play, reverse direction when finnished),
        /// ATYPE_ST_SWITCH (parameter for ATYPE_ALL_ONCE: switch state when finnished).
        /// The appended _REV means that the animation is started at the end and played backwards.
        Uint16 animation_type;
        /// FPS of the animation
        double fps;
        /// Start position from the frame array of the base_image
        Uint16 start_pos;
        ///\brief Allignment of the base point onto the base_pos
        ///
        /// The allignment specifies how the base point should be positioned wrt to the
        /// base_pos: It's type are basically the same as in BasePointType.
        ///
        /// Example: (Assuming BP_MD) AT_LD means the middle point of each frame (BP_MD)
        ///          is stays fixed wrt to the base position and the animation is placed with
        ///          a "south west gravity" (similar to ImageMagick) wrt to the base position
        /// Default: AT_MD
        AllignType allign_type;
        
        /* calculated (checkAnim()) or given values */
        /// Pointer to the current base position of the animation
        SDL_Rect* base_pos;
        /// shift is added as an additional value to the designated BP position.
        Sint16 shift_x;
        Sint16 shift_y;
        /// Duration of the animation in ms
        Uint32 duration;
        /// Position of the base frame
        Uint16 base_frame_pos;
        /// End position from the frame array of the base_image
        Uint16 end_pos;
        /// True if the animation consist of one still image (frames=1,still=true)
        bool is_image;
        /// True if the sanity checks suceeded
        bool is_valid;
        /// True if the image dimensions stay fixed during the whole animation (unused)
        bool is_fixed;

        /* temporary values */
        /// True if the animation is running
        bool is_running;
        /// True if the animation is running forward at the moment
        bool forward;
        /// Current frame number
        Uint16 cur_frame_num;
        /// Current time
        Uint32 cur_time;
};

#endif
